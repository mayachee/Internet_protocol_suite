#include "ft_ping.h"

static volatile sig_atomic_t g_stop;

static void handle_sigint(int signal_number)
{
	(void)signal_number;
	g_stop = 1;
}

static double elapsed_milliseconds(const struct timespec *start,
	const struct timespec *end)
{
	time_t seconds;
	long nanoseconds;

	seconds = end->tv_sec - start->tv_sec;
	nanoseconds = end->tv_nsec - start->tv_nsec;
	return (double)seconds * 1000.0 + (double)nanoseconds / 1000000.0;
}

int run_ping(const struct s_ping_options *options,
	const struct sockaddr_in *address)
{
	struct sigaction action;
	struct sockaddr_in sender;
	struct timespec sent_at;
	struct timespec received_at;
	struct timespec now;
	struct timeval timeout;
	fd_set read_set;
	struct ip *ip_header;
	struct icmphdr *icmp_header;
	uint8_t packet[sizeof(struct icmphdr) + FT_PING_DATA_SIZE];
	uint8_t received[4096];
	char text[INET_ADDRSTRLEN];
	socklen_t sender_length;
	ssize_t received_size;
	int socket_fd;
	int ready;
	int got_reply;
	unsigned int transmitted;
	unsigned int replies;
	uint16_t identifier;
	uint16_t sequence;

	socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (socket_fd < 0)
	{
		perror("ft_ping: socket");
		return 1;
	}
	memset(&action, 0, sizeof(action));
	action.sa_handler = handle_sigint;
	sigemptyset(&action.sa_mask);
	if (sigaction(SIGINT, &action, NULL) < 0)
	{
		perror("ft_ping: sigaction");
		close(socket_fd);
		return 1;
	}
	if (inet_ntop(AF_INET, &address->sin_addr, text, sizeof(text)) == NULL)
	{
		perror("ft_ping: inet_ntop");
		close(socket_fd);
		return 1;
	}
	printf("PING %s (%s): %d data bytes\n", options->target, text,
		FT_PING_DATA_SIZE);
	identifier = (uint16_t)getpid();
	sequence = 0;
	transmitted = 0;
	replies = 0;
	while (!g_stop)
	{
		if (build_echo_request(packet, sizeof(packet), identifier, sequence)
			!= 0)
			break;
		icmp_header = (struct icmphdr *)packet;
		icmp_header->un.echo.id = htons(identifier);
		icmp_header->un.echo.sequence = htons(sequence);
		icmp_header->checksum = 0;
		icmp_header->checksum = htons(icmp_checksum(packet, sizeof(packet)));
		clock_gettime(CLOCK_MONOTONIC, &sent_at);
		if (sendto(socket_fd, packet, sizeof(packet), 0,
				(const struct sockaddr *)address, sizeof(*address)) < 0)
		{
			if (errno == EINTR && g_stop)
				break;
			perror("ft_ping: sendto");
			close(socket_fd);
			return 1;
		}
		transmitted++;
		got_reply = 0;
		while (!got_reply && !g_stop)
		{
			clock_gettime(CLOCK_MONOTONIC, &now);
			{
				double remaining;

				remaining = 1.0 - elapsed_milliseconds(&sent_at, &now)
					/ 1000.0;
				if (remaining <= 0.0)
					break;
				timeout.tv_sec = (time_t)remaining;
				timeout.tv_usec = (suseconds_t)((remaining
						- (double)timeout.tv_sec) * 1000000.0);
			}
			FD_ZERO(&read_set);
			FD_SET(socket_fd, &read_set);
			ready = select(socket_fd + 1, &read_set, NULL, NULL, &timeout);
			if (ready < 0)
			{
				if (errno == EINTR)
					continue;
				perror("ft_ping: select");
				close(socket_fd);
				return 1;
			}
			if (ready == 0)
				break;
			sender_length = sizeof(sender);
			received_size = recvfrom(socket_fd, received, sizeof(received), 0,
				(struct sockaddr *)&sender, &sender_length);
			if (received_size < 0)
			{
				if (errno != EINTR)
					perror("ft_ping: recvfrom");
				continue;
			}
			if ((size_t)received_size < sizeof(struct ip)
				|| (size_t)received_size < sizeof(struct ip)
					+ sizeof(struct icmphdr))
				continue;
			ip_header = (struct ip *)received;
			if (ip_header->ip_hl < 5
				|| (size_t)received_size < (size_t)ip_header->ip_hl * 4
					+ sizeof(struct icmphdr))
				continue;
			icmp_header = (struct icmphdr *)(received + ip_header->ip_hl * 4);
			if (icmp_header->type == ICMP_ECHOREPLY
				&& ntohs(icmp_header->un.echo.id) == identifier
				&& ntohs(icmp_header->un.echo.sequence) == sequence)
			{
				clock_gettime(CLOCK_MONOTONIC, &received_at);
				printf("%zu bytes from %s: icmp_seq=%u ttl=%u "
					"time=%.3f ms\n",
					(size_t)received_size - ip_header->ip_hl * 4,
					text, sequence, ip_header->ip_ttl,
					elapsed_milliseconds(&sent_at, &received_at));
				replies++;
				got_reply = 1;
			}
		}
		if (!got_reply)
			printf("Request timeout for icmp_seq %u\n", sequence);
		sequence++;
		if (!g_stop)
			sleep(1);
	}
	print_statistics(transmitted, replies);
	close(socket_fd);
	return 0;
}

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

static void print_icmp_error(const struct icmphdr *header)
{
	const char *message;

	message = NULL;
	if (header->type == ICMP_DEST_UNREACH)
		message = "Destination Unreachable";
	else if (header->type == ICMP_TIME_EXCEEDED)
		message = "Time to live exceeded";
	else if (header->type == ICMP_REDIRECT)
		message = "Redirect";
	if (message != NULL)
		printf("From: %s\n", message);
}

static int configure_socket(int socket_fd, const struct s_ping_options *options)
{
	unsigned char timestamp[40];

	if (options->bypass_route
		&& setsockopt(socket_fd, SOL_SOCKET, SO_DONTROUTE,
			&(int){1}, sizeof(int)) < 0)
	{
		perror("ft_ping: SO_DONTROUTE");
		return -1;
	}
	if (setsockopt(socket_fd, IPPROTO_IP, IP_TTL, &options->ttl,
			sizeof(options->ttl)) < 0)
	{
		perror("ft_ping: IP_TTL");
		return -1;
	}
	if (options->ip_timestamp)
	{
		memset(timestamp, 0, sizeof(timestamp));
		timestamp[0] = IPOPT_TIMESTAMP;
		timestamp[1] = sizeof(timestamp);
		timestamp[2] = 5;
		timestamp[3] = 0;
		if (setsockopt(socket_fd, IPPROTO_IP, IP_OPTIONS, timestamp,
				sizeof(timestamp)) < 0)
		{
			perror("ft_ping: IP_OPTIONS");
			return -1;
		}
	}
	return 0;
}

static void set_timeout(struct timeval *timeout, double seconds)
{
	timeout->tv_sec = (time_t)seconds;
	timeout->tv_usec = (suseconds_t)((seconds - (double)timeout->tv_sec)
			* 1000000.0);
}

static void wait_until_next_request(const struct timespec *sent_at,
	const struct s_ping_options *options)
{
	struct timespec now;
	struct timespec delay;
	double remaining;

	if (options->flood)
		return;
	clock_gettime(CLOCK_MONOTONIC, &now);
	remaining = 1.0 - elapsed_milliseconds(sent_at, &now) / 1000.0;
	if (remaining <= 0.0)
		return;
	delay.tv_sec = (time_t)remaining;
	delay.tv_nsec = (long)((remaining - (double)delay.tv_sec) * 1000000000.0);
	while (nanosleep(&delay, &delay) < 0 && errno == EINTR && !g_stop)
		;
}

int run_ping(const struct s_ping_options *options,
	const struct sockaddr_in *address)
{
	struct sigaction action;
	struct sockaddr_in sender;
	struct timespec sent_at;
	struct timespec received_at;
	struct timespec now;
	struct timespec started_at;
	struct timeval timeout;
	fd_set read_set;
	struct ip *ip_header;
	struct icmphdr *icmp_header;
	size_t packet_length;
	uint8_t *packet;
	uint8_t received_packet[65536];
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
	double *rtt;

	packet_length = sizeof(struct icmphdr) + options->packet_size;
	packet = malloc(packet_length);
	rtt = malloc(sizeof(*rtt));
	if (packet == NULL || rtt == NULL)
	{
		perror("ft_ping: malloc");
		free(packet);
		free(rtt);
		return 1;
	}
	socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (socket_fd < 0)
	{
		perror("ft_ping: socket");
		free(packet);
		free(rtt);
		return 1;
	}
	if (configure_socket(socket_fd, options) != 0)
	{
		close(socket_fd);
		free(packet);
		free(rtt);
		return 1;
	}
	memset(&action, 0, sizeof(action));
	action.sa_handler = handle_sigint;
	sigemptyset(&action.sa_mask);
	if (sigaction(SIGINT, &action, NULL) < 0)
	{
		perror("ft_ping: sigaction");
		close(socket_fd);
		free(packet);
		free(rtt);
		return 1;
	}
	if (inet_ntop(AF_INET, &address->sin_addr, text, sizeof(text)) == NULL)
	{
		perror("ft_ping: inet_ntop");
		close(socket_fd);
		free(packet);
		free(rtt);
		return 1;
	}
	printf("PING %s (%s): %u data bytes\n", options->target, text,
		options->packet_size);
	clock_gettime(CLOCK_MONOTONIC, &started_at);
	identifier = (uint16_t)getpid();
	sequence = 0;
	transmitted = 0;
	replies = 0;
	while (!g_stop)
	{
		clock_gettime(CLOCK_MONOTONIC, &now);
		if (options->deadline > 0
			&& ((options->flood
					&& elapsed_milliseconds(&started_at, &now)
						>= (double)options->deadline * 1000.0)
				|| (!options->flood && transmitted > options->deadline)))
			break;
		if (build_echo_request(packet, packet_length, identifier, sequence,
				options) != 0)
			break;
		icmp_header = (struct icmphdr *)packet;
		icmp_header->un.echo.id = htons(identifier);
		icmp_header->un.echo.sequence = htons(sequence);
		icmp_header->checksum = 0;
		icmp_header->checksum = htons(icmp_checksum(packet, packet_length));
		clock_gettime(CLOCK_MONOTONIC, &sent_at);
		if (sendto(socket_fd, packet, packet_length, 0,
				(const struct sockaddr *)address, sizeof(*address)) < 0)
		{
			if (errno == EINTR && g_stop)
				break;
			perror("ft_ping: sendto");
			close(socket_fd);
			free(packet);
			free(rtt);
			return 1;
		}
		transmitted++;
		got_reply = 0;
		while (!got_reply && !g_stop)
		{
			clock_gettime(CLOCK_MONOTONIC, &now);
			{
				double remaining;

				remaining = options->timeout
					- elapsed_milliseconds(&sent_at, &now) / 1000.0;
				if (remaining <= 0.0)
					break;
				set_timeout(&timeout, remaining);
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
				free(packet);
				free(rtt);
				return 1;
			}
			if (ready == 0)
				break;
			sender_length = sizeof(sender);
			received_size = recvfrom(socket_fd, received_packet,
				sizeof(received_packet), 0, (struct sockaddr *)&sender,
				&sender_length);
			if (received_size < 0)
			{
				if (errno != EINTR)
					perror("ft_ping: recvfrom");
				continue;
			}
			if ((size_t)received_size < sizeof(struct ip))
				continue;
			ip_header = (struct ip *)received_packet;
			if (ip_header->ip_hl < 5
				|| (size_t)received_size < (size_t)ip_header->ip_hl * 4
					+ sizeof(struct icmphdr))
				continue;
			icmp_header = (struct icmphdr *)(received_packet
					+ ip_header->ip_hl * 4);
			if (icmp_header->type == ICMP_ECHOREPLY
				&& ntohs(icmp_header->un.echo.id) == identifier
				&& ntohs(icmp_header->un.echo.sequence) == sequence)
			{
				clock_gettime(CLOCK_MONOTONIC, &received_at);
				if (replies > 0)
				{
					double *new_rtt;

					new_rtt = realloc(rtt, sizeof(*rtt) * (replies + 1));
					if (new_rtt == NULL)
					{
						perror("ft_ping: realloc");
						close(socket_fd);
						free(packet);
						free(rtt);
						return 1;
					}
					rtt = new_rtt;
				}
				rtt[replies] = elapsed_milliseconds(&sent_at, &received_at);
				if (!options->flood)
					printf("%zu bytes from %s: icmp_seq=%u ttl=%u "
						"time=%.3f ms\n",
						(size_t)received_size - ip_header->ip_hl * 4,
						text, sequence, ip_header->ip_ttl, rtt[replies]);
				else
					printf("\b \b");
				replies++;
				got_reply = 1;
			}
			else if (options->verbose)
				print_icmp_error(icmp_header);
		}
		if (!got_reply && options->flood)
			printf("#");
		sequence++;
		if (!g_stop && sequence >= options->preload)
			wait_until_next_request(&sent_at, options);
	}
	if (options->flood)
		printf("\n");
	printf("--- %s ping statistics ---\n", options->target);
	print_statistics(transmitted, replies, rtt, replies);
	close(socket_fd);
	free(packet);
	free(rtt);
	return 0;
}

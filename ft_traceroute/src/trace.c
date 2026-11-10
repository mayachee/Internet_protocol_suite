#include "ft_traceroute.h"

static volatile sig_atomic_t g_stop;

static void	handle_signal(int signal_number)
{
	(void)signal_number;
	g_stop = 1;
}

static double	elapsed_ms(const struct timespec *start,
	const struct timespec *end)
{
	return ((double)(end->tv_sec - start->tv_sec) * 1000.0
		+ (double)(end->tv_nsec - start->tv_nsec) / 1000000.0);
}

static void	set_timeout(struct timeval *timeout, double seconds)
{
	timeout->tv_sec = (time_t)seconds;
	timeout->tv_usec = (suseconds_t)((seconds - (double)timeout->tv_sec)
			* 1000000.0);
}

static int	build_probe(uint8_t *packet, uint16_t identifier,
	uint16_t sequence, size_t packet_size)
{
	struct icmphdr	*header;

	memset(packet, 0, packet_size);
	header = (struct icmphdr *)packet;
	header->type = ICMP_ECHO;
	header->un.echo.id = htons(identifier);
	header->un.echo.sequence = htons(sequence);
	header->checksum = htons(icmp_checksum(packet,
				packet_size));
	return (0);
}

static int	matches_probe(const uint8_t *packet, size_t packet_size,
	uint16_t identifier, uint16_t sequence, int *destination_reached)
{
	const struct ip		*outer_ip;
	const struct icmphdr	*icmp;
	const struct ip		*inner_ip;
	const struct icmphdr	*inner_icmp;
	size_t			outer_length;
	size_t			inner_length;

	if (packet_size < sizeof(struct ip))
		return (0);
	outer_ip = (const struct ip *)packet;
	outer_length = (size_t)outer_ip->ip_hl * 4;
	if (outer_ip->ip_hl < 5
		|| packet_size < outer_length + sizeof(struct icmphdr))
		return (0);
	icmp = (const struct icmphdr *)(packet + outer_length);
	if (icmp->type == ICMP_ECHOREPLY
		&& ntohs(icmp->un.echo.id) == identifier
		&& ntohs(icmp->un.echo.sequence) == sequence)
	{
		*destination_reached = 1;
		return (1);
	}
	if (icmp->type != ICMP_TIME_EXCEEDED
		&& icmp->type != ICMP_DEST_UNREACH)
		return (0);
	inner_length = outer_length + sizeof(struct icmphdr);
	if (packet_size < inner_length + sizeof(struct ip))
		return (0);
	inner_ip = (const struct ip *)(packet + inner_length);
	inner_length += (size_t)inner_ip->ip_hl * 4;
	if (inner_ip->ip_hl < 5
		|| packet_size < inner_length + sizeof(struct icmphdr))
		return (0);
	inner_icmp = (const struct icmphdr *)(packet + inner_length);
	if (inner_icmp->type != ICMP_ECHO
		|| ntohs(inner_icmp->un.echo.id) != identifier
		|| ntohs(inner_icmp->un.echo.sequence) != sequence)
		return (0);
	if (icmp->type == ICMP_DEST_UNREACH)
		*destination_reached = 1;
	return (1);
}

static int	receive_probe(int socket_fd, uint16_t identifier, uint16_t sequence,
	struct sockaddr_in *sender, double *milliseconds, int *destination_reached)
{
	uint8_t			packet[65536];
	struct timespec		start;
	struct timespec		now;
	struct timespec		end;
	struct timeval		timeout;
	fd_set			read_set;
	socklen_t		sender_length;
	ssize_t			packet_size;
	int				ready;

	clock_gettime(CLOCK_MONOTONIC, &start);
	while (!g_stop)
	{
		clock_gettime(CLOCK_MONOTONIC, &now);
		if (elapsed_ms(&start, &now) >= FT_TRACEROUTE_TIMEOUT * 1000.0)
			return (0);
		set_timeout(&timeout, FT_TRACEROUTE_TIMEOUT
			- elapsed_ms(&start, &now) / 1000.0);
		FD_ZERO(&read_set);
		FD_SET(socket_fd, &read_set);
		ready = select(socket_fd + 1, &read_set, NULL, NULL, &timeout);
		if (ready < 0)
		{
			if (errno == EINTR)
				continue;
			return (-1);
		}
		if (ready == 0)
			return (0);
		sender_length = sizeof(*sender);
		packet_size = recvfrom(socket_fd, packet, sizeof(packet), 0,
				(struct sockaddr *)sender, &sender_length);
		if (packet_size < 0)
		{
			if (errno == EINTR)
				continue;
			return (-1);
		}
		if (matches_probe(packet, (size_t)packet_size, identifier, sequence,
				destination_reached))
		{
			clock_gettime(CLOCK_MONOTONIC, &end);
			*milliseconds = elapsed_ms(&start, &end);
			return (1);
		}
	}
	return (0);
}

static void	print_hop_address(const struct sockaddr_in *sender)
{
	char address[INET_ADDRSTRLEN];

	if (inet_ntop(AF_INET, &sender->sin_addr, address, sizeof(address)) != NULL)
		printf("%s", address);
	else
		printf("?");
}

int	run_traceroute(const struct s_trace_options *options,
	const struct sockaddr_in *destination)
{
	struct sigaction	action;
	struct sockaddr_in	sender;
	char				destination_text[INET_ADDRSTRLEN];
	uint8_t				*packet;
	size_t				packet_size;
	uint16_t			identifier;
	uint16_t			sequence;
	double				milliseconds;
	int					socket_fd;
	int					reached;
	int					result;
	int					ttl;
	int					probe;
	int					printed_address;

	packet_size = sizeof(struct icmphdr) + options->packet_size;
	packet = malloc(packet_size);
	if (packet == NULL)
	{
		perror("ft_traceroute: malloc");
		return (1);
	}
	socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (socket_fd < 0)
	{
		perror("ft_traceroute: socket");
		free(packet);
		return (1);
	}
	if (options->interface[0] != '\0'
		&& setsockopt(socket_fd, SOL_SOCKET, SO_BINDTODEVICE,
			options->interface, strlen(options->interface) + 1) < 0)
	{
		perror("ft_traceroute: SO_BINDTODEVICE");
		close(socket_fd);
		free(packet);
		return (1);
	}
	if (setsockopt(socket_fd, IPPROTO_IP, IP_TOS, &options->tos,
			sizeof(options->tos)) < 0)
	{
		perror("ft_traceroute: IP_TOS");
		close(socket_fd);
		free(packet);
		return (1);
	}
	memset(&action, 0, sizeof(action));
	action.sa_handler = handle_signal;
	sigemptyset(&action.sa_mask);
	if (sigaction(SIGINT, &action, NULL) < 0)
	{
		perror("ft_traceroute: sigaction");
		close(socket_fd);
		free(packet);
		return (1);
	}
	if (inet_ntop(AF_INET, &destination->sin_addr, destination_text,
			sizeof(destination_text)) == NULL)
	{
		perror("ft_traceroute: inet_ntop");
		close(socket_fd);
		free(packet);
		return (1);
	}
	printf("traceroute to %s (%s), %u hops max\n", options->target,
		destination_text, options->max_hops);
	identifier = (uint16_t)(getpid() ^ options->port);
	sequence = 0;
	reached = 0;
	for (ttl = 1; ttl <= (int)options->max_hops && !g_stop && !reached; ttl++)
	{
		printf("%2d  ", ttl);
		printed_address = 0;
		for (probe = 0; probe < (int)options->probes; probe++)
		{
			if (setsockopt(socket_fd, IPPROTO_IP, IP_TTL, &ttl,
					sizeof(ttl)) < 0)
			{
				perror("ft_traceroute: IP_TTL");
				close(socket_fd);
				return (1);
			}
			build_probe(packet, identifier, sequence, packet_size);
			if (sendto(socket_fd, packet, sizeof(packet), 0,
					(const struct sockaddr *)destination,
					sizeof(*destination)) < 0)
			{
				if (errno == EINTR && g_stop)
					break;
				perror("ft_traceroute: sendto");
				close(socket_fd);
				return (1);
			}
			result = receive_probe(socket_fd, identifier, sequence, &sender,
					&milliseconds, &reached);
			if (result < 0)
			{
				perror("ft_traceroute: receive");
				close(socket_fd);
				return (1);
			}
			if (result == 0)
				printf("* ");
			else
			{
				if (!printed_address)
				{
					print_hop_address(&sender);
					printed_address = 1;
				}
				else
					printf("                 ");
				printf("  %.3f ms ", milliseconds);
			}
			sequence++;
		}
		printf("\n");
	}
	close(socket_fd);
	free(packet);
	return (0);
}

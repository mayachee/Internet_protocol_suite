#ifndef FT_PING_H
# define FT_PING_H

# include <netinet/in.h>
# include <stddef.h>
# include <stdint.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

# define FT_PING_DATA_SIZE 56

struct s_ping_options
{
	int verbose;
	int show_help;
	const char *target;
};

int		parse_args(int argc, char **argv, struct s_ping_options *options);
int		resolve_host(const char *target, struct sockaddr_in *address);
uint16_t	icmp_checksum(const void *data, size_t length);
int		build_echo_request(uint8_t *packet, size_t packet_size, uint16_t identifier,
			uint16_t sequence);
int		run_ping(const struct s_ping_options *options,
			const struct sockaddr_in *address);
void		print_reply(const struct sockaddr_in *address, const uint8_t *packet,
			size_t packet_size);
void		print_statistics(unsigned int transmitted, unsigned int received);

#endif

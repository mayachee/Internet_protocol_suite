#ifndef FT_TRACEROUTE_H
# define FT_TRACEROUTE_H

# include <arpa/inet.h>
# include <errno.h>
# include <netdb.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <net/if.h>
# include <signal.h>
# include <stddef.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/select.h>
# include <sys/socket.h>
# include <time.h>
# include <unistd.h>

# define FT_TRACEROUTE_MAX_HOPS 30
# define FT_TRACEROUTE_PROBES 3
# define FT_TRACEROUTE_TIMEOUT 1.0
# define FT_TRACEROUTE_DATA_SIZE 32
# define FT_TRACEROUTE_MAX_PACKET 65507

struct s_trace_options
{
	int show_help;
	unsigned int max_hops;
	unsigned int probes;
	unsigned int packet_size;
	unsigned int simultaneous;
	unsigned int tos;
	unsigned int port;
	unsigned int flow_label;
	char interface[IFNAMSIZ];
	const char *target;
};

int		parse_args(int argc, char **argv, struct s_trace_options *options);
int		resolve_host(const char *target, struct sockaddr_in *address);
uint16_t	icmp_checksum(const void *data, size_t length);
int		run_traceroute(const struct s_trace_options *options,
			const struct sockaddr_in *destination);

#endif

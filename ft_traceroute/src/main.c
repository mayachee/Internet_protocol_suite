#include "ft_traceroute.h"

int	main(int argc, char **argv)
{
	struct s_trace_options options;
	struct sockaddr_in destination;
	int result;

	memset(&options, 0, sizeof(options));
	result = parse_args(argc, argv, &options);
	if (result != 0)
		return (result < 0);
	if (resolve_host(options.target, &destination) != 0)
		return (1);
	return (run_traceroute(&options, &destination));
}

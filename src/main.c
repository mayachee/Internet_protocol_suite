#include "ft_ping.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	struct s_ping_options options;
	struct sockaddr_in address;
	int result;

	memset(&options, 0, sizeof(options));
	result = parse_args(argc, argv, &options);
	if (result != 0)
		return result < 0 ? 1 : 0;
	if (resolve_host(options.target, &address) != 0)
		return 1;
	return run_ping(&options, &address);
}

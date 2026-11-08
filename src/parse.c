#include "ft_ping.h"

#include <stdio.h>
#include <string.h>

static void print_help(const char *program)
{
	printf("Usage: %s [-v] [-?] <host>\n", program);
	printf("  -v    show ICMP error messages\n");
	printf("  -?    show this help message\n");
}

int parse_args(int argc, char **argv, struct s_ping_options *options)
{
	int index;

	if (argc < 2)
	{
		print_help(argv[0]);
		return -1;
	}
	index = 1;
	while (index < argc && argv[index][0] == '-')
	{
		if (strcmp(argv[index], "-v") == 0)
			options->verbose = 1;
		else if (strcmp(argv[index], "-?") == 0)
		{
			print_help(argv[0]);
			return 1;
		}
		else
		{
			fprintf(stderr, "%s: unknown option: %s\n", argv[0], argv[index]);
			return -1;
		}
		index++;
	}
	if (index != argc - 1)
	{
		fprintf(stderr, "%s: one host is required\n", argv[0]);
		return -1;
	}
	options->target = argv[index];
	return 0;
}

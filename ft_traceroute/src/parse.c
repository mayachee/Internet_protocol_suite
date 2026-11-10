#include "ft_traceroute.h"

#include <limits.h>

static void	print_help(const char *program)
{
	printf("Usage: %s [options] <IPv4 address | hostname | FQDN>\n", program);
	printf("  --help          display this help\n");
	printf("  -i interface    use a network interface\n");
	printf("  -m hops         maximum hop limit (default: 30)\n");
	printf("  -p port         probe port (default: 33434)\n");
	printf("  -s size         ICMP payload size (default: 32)\n");
	printf("  -q count        probes per hop (default: 3)\n");
	printf("  -N count        maximum simultaneous probes\n");
	printf("  -t tos          IPv4 Type of Service (0-255)\n");
	printf("  -l label        IPv4 flow-label value\n");
}

static int	parse_number(const char *text, unsigned int minimum,
	unsigned int maximum, unsigned int *value)
{
	char *end;
	unsigned long number;

	errno = 0;
	number = strtoul(text, &end, 10);
	if (text[0] == '\0' || *end != '\0' || errno == ERANGE
		|| number < minimum || number > maximum)
		return (-1);
	*value = (unsigned int)number;
	return (0);
}

static int	next_value(int argc, char **argv, int *index, const char **value)
{
	if (*index + 1 >= argc)
		return (-1);
	*value = argv[++(*index)];
	return (0);
}

int	parse_args(int argc, char **argv, struct s_trace_options *options)
{
	const char	*value;
	unsigned int	parsed;
	int			index;

	options->max_hops = FT_TRACEROUTE_MAX_HOPS;
	options->probes = FT_TRACEROUTE_PROBES;
	options->packet_size = FT_TRACEROUTE_DATA_SIZE;
	options->simultaneous = 1;
	options->port = 33434;
	if (argc == 2 && strcmp(argv[1], "--help") == 0)
	{
		options->show_help = 1;
		print_help(argv[0]);
		return (1);
	}
	index = 1;
	while (index < argc && argv[index][0] == '-')
	{
		if (strcmp(argv[index], "--help") == 0)
		{
			print_help(argv[0]);
			return (1);
		}
		if (strcmp(argv[index], "-i") == 0)
		{
			if (next_value(argc, argv, &index, &value) != 0
				|| strlen(value) >= sizeof(options->interface))
				goto invalid;
			strcpy(options->interface, value);
		}
		else if (strcmp(argv[index], "-m") == 0)
		{
			if (next_value(argc, argv, &index, &value) != 0
				|| parse_number(value, 1, 255, &options->max_hops) != 0)
				goto invalid;
		}
		else if (strcmp(argv[index], "-p") == 0)
		{
			if (next_value(argc, argv, &index, &value) != 0
				|| parse_number(value, 1, 65535, &options->port) != 0)
				goto invalid;
		}
		else if (strcmp(argv[index], "-s") == 0)
		{
			if (next_value(argc, argv, &index, &value) != 0
				|| parse_number(value, 0, FT_TRACEROUTE_MAX_PACKET,
					&options->packet_size) != 0)
				goto invalid;
		}
		else if (strcmp(argv[index], "-q") == 0)
		{
			if (next_value(argc, argv, &index, &value) != 0
				|| parse_number(value, 1, 100, &options->probes) != 0)
				goto invalid;
		}
		else if (strcmp(argv[index], "-N") == 0)
		{
			if (next_value(argc, argv, &index, &value) != 0
				|| parse_number(value, 1, 100, &options->simultaneous) != 0)
				goto invalid;
		}
		else if (strcmp(argv[index], "-t") == 0
			|| strcmp(argv[index], "-l") == 0)
		{
			int is_flow_label;

			is_flow_label = strcmp(argv[index], "-l") == 0;
			if (next_value(argc, argv, &index, &value) != 0
				|| parse_number(value, 0, is_flow_label ? 1048575 : 255,
					&parsed) != 0)
				goto invalid;
			if (is_flow_label)
				options->flow_label = parsed;
			else
				options->tos = parsed;
		}
		else
		{
			fprintf(stderr, "%s: unknown option: %s\n", argv[0], argv[index]);
			return (-1);
		}
		index++;
	}
	if (index != argc - 1)
	{
		fprintf(stderr, "%s: exactly one destination is required\n", argv[0]);
		return (-1);
	}
	options->target = argv[index];
	return (0);
invalid:
	fprintf(stderr, "%s: invalid value for option\n", argv[0]);
	return (-1);
}

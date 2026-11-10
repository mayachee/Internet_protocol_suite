#include "ft_ping.h"

static void print_help(const char *program)
{
	printf("Usage: %s [options] <host>\n", program);
	printf("  -v              show ICMP error messages\n");
	printf("  -?              show this help message\n");
	printf("  -f              flood mode\n");
	printf("  -l count        send count packets before waiting\n");
	printf("  -n              numeric output\n");
	printf("  -w deadline     stop after deadline seconds\n");
	printf("  -W timeout      wait timeout seconds for each reply\n");
	printf("  -p pattern      hexadecimal payload pattern\n");
	printf("  -r              bypass routing tables\n");
	printf("  -s size         ICMP payload size in bytes\n");
	printf("  -T ttl          set IPv4 time-to-live\n");
	printf("  --ip-timestamp  request an IPv4 timestamp option\n");
}

static int parse_unsigned(const char *text, unsigned int minimum,
	unsigned int maximum, unsigned int *value)
{
	char *end;
	unsigned long parsed;

	errno = 0;
	parsed = strtoul(text, &end, 10);
	if (text[0] == '\0' || *end != '\0' || errno == ERANGE
		|| parsed < minimum || parsed > maximum)
		return -1;
	*value = (unsigned int)parsed;
	return 0;
}

static int parse_double(const char *text, double *value)
{
	char *end;
	double parsed;

	errno = 0;
	parsed = strtod(text, &end);
	if (text[0] == '\0' || *end != '\0' || errno == ERANGE
		|| parsed <= 0.0)
		return -1;
	*value = parsed;
	return 0;
}

static int hex_value(char character)
{
	if (character >= '0' && character <= '9')
		return character - '0';
	if (character >= 'a' && character <= 'f')
		return character - 'a' + 10;
	if (character >= 'A' && character <= 'F')
		return character - 'A' + 10;
	return -1;
}

static int parse_pattern(const char *text, struct s_ping_options *options)
{
	size_t length;
	int high;
	int low;

	length = strlen(text);
	if (length == 0 || length > FT_PING_PATTERN_SIZE * 2
		|| length % 2 != 0)
		return -1;
	options->pattern_size = length / 2;
	for (size_t index = 0; index < options->pattern_size; index++)
	{
		high = hex_value(text[index * 2]);
		low = hex_value(text[index * 2 + 1]);
		if (high < 0 || low < 0)
			return -1;
		options->pattern[index] = (uint8_t)((high << 4) | low);
	}
	return 0;
}

static int option_value(int argc, char **argv, int *index,
	const char **value)
{
	if (*index + 1 >= argc)
		return -1;
	(*index)++;
	*value = argv[*index];
	return 0;
}

int parse_args(int argc, char **argv, struct s_ping_options *options)
{
	const char *value;
	int index;

	options->packet_size = FT_PING_DATA_SIZE;
	options->timeout = 1.0;
	options->ttl = 64;
	options->pattern_size = 0;
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
		else if (strcmp(argv[index], "-f") == 0)
			options->flood = 1;
		else if (strcmp(argv[index], "-n") == 0)
			options->numeric = 1;
		else if (strcmp(argv[index], "-r") == 0)
			options->bypass_route = 1;
		else if (strcmp(argv[index], "--ip-timestamp") == 0)
			options->ip_timestamp = 1;
		else if (strcmp(argv[index], "-?") == 0)
		{
			print_help(argv[0]);
			return 1;
		}
		else if (strcmp(argv[index], "-l") == 0
			|| strcmp(argv[index], "-w") == 0
			|| strcmp(argv[index], "-W") == 0
			|| strcmp(argv[index], "-p") == 0
			|| strcmp(argv[index], "-s") == 0
			|| strcmp(argv[index], "-T") == 0
			|| strcmp(argv[index], "--ttl") == 0)
		{
			if (option_value(argc, argv, &index, &value) != 0)
			{
				fprintf(stderr, "%s: option requires an argument: %s\n",
					argv[0], argv[index]);
				return -1;
			}
			if (strcmp(argv[index - 1], "-l") == 0
				&& parse_unsigned(value, 0, UINT_MAX, &options->preload) != 0)
				goto invalid_value;
			if (strcmp(argv[index - 1], "-s") == 0
				&& parse_unsigned(value, 0, 65507, &options->packet_size) != 0)
				goto invalid_value;
			if ((strcmp(argv[index - 1], "-T") == 0
					|| strcmp(argv[index - 1], "--ttl") == 0)
				&& parse_unsigned(value, 1, 255, &options->ttl) != 0)
				goto invalid_value;
			if (strcmp(argv[index - 1], "-w") == 0
				&& parse_unsigned(value, 1, UINT_MAX, &options->deadline) != 0)
				goto invalid_value;
			if (strcmp(argv[index - 1], "-W") == 0
				&& parse_double(value, &options->timeout) != 0)
				goto invalid_value;
			if (strcmp(argv[index - 1], "-p") == 0
				&& parse_pattern(value, options) != 0)
				goto invalid_value;
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
invalid_value:
	fprintf(stderr, "%s: invalid value for %s\n", argv[0], argv[index - 1]);
	return -1;
}

#include "ft_ping.h"

int resolve_host(const char *target, struct sockaddr_in *address)
{
	struct addrinfo hints;
	struct addrinfo *result;
	int status;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	status = getaddrinfo(target, NULL, &hints, &result);
	if (status != 0)
	{
		fprintf(stderr, "ft_ping: %s: %s\n", target, gai_strerror(status));
		return -1;
	}
	memcpy(address, result->ai_addr, sizeof(*address));
	freeaddrinfo(result);
	return 0;
}

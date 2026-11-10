#include "ft_traceroute.h"

int	resolve_host(const char *target, struct sockaddr_in *address)
{
	struct addrinfo hints;
	struct addrinfo *result;
	// char address_text[INET_ADDRSTRLEN];
	int status;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	status = getaddrinfo(target, NULL, &hints, &result);
	if (status != 0)
	{
		fprintf(stderr, "ft_traceroute: %s: %s\n", target,
			gai_strerror(status));
		return (-1);
	}
	memcpy(address, result->ai_addr, sizeof(*address));
	// if (inet_ntop(AF_INET, &address->sin_addr, address_text,
	// 		sizeof(address_text)) == NULL)
	// {
	// 	perror("ft_traceroute: inet_ntop");
	// 	freeaddrinfo(result);
	// 	return (-1);
	// }
	// printf("Address: %s, Port: %u, Family: %d\n", address_text,
	// 	(unsigned int)ntohs(address->sin_port), address->sin_family);
	freeaddrinfo(result);
	return (0);
}

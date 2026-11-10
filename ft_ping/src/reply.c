#include "ft_ping.h"

void print_reply(const struct sockaddr_in *address, const uint8_t *packet,
	size_t packet_size)
{
	char text[INET_ADDRSTRLEN];

	(void)packet;
	(void)packet_size;
	if (inet_ntop(AF_INET, &address->sin_addr, text, sizeof(text)) != NULL)
		printf("reply from %s\n", text);
}

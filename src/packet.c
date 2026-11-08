#include "ft_ping.h"

#include <netinet/ip_icmp.h>
#include <string.h>

uint16_t icmp_checksum(const void *data, size_t length)
{
	const uint8_t *bytes;
	uint32_t sum;

	bytes = data;
	sum = 0;
	while (length > 1)
	{
		sum += ((uint16_t)bytes[0] << 8) | bytes[1];
		bytes += 2;
		length -= 2;
	}
	if (length == 1)
		sum += (uint16_t)bytes[0] << 8;
	while (sum >> 16)
		sum = (sum & 0xffffU) + (sum >> 16);
	return (uint16_t)~sum;
}

int build_echo_request(uint8_t *packet, size_t packet_size, uint16_t identifier,
	uint16_t sequence)
{
	struct icmphdr *header;

	if (packet_size < sizeof(*header) + FT_PING_DATA_SIZE)
		return -1;
	memset(packet, 0, packet_size);
	header = (struct icmphdr *)packet;
	header->type = ICMP_ECHO;
	header->code = 0;
	header->un.echo.id = identifier;
	header->un.echo.sequence = sequence;
	header->checksum = icmp_checksum(packet, sizeof(*header) + FT_PING_DATA_SIZE);
	return 0;
}

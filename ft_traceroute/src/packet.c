#include "ft_traceroute.h"

uint16_t	icmp_checksum(const void *data, size_t length)
{
	const uint8_t	*bytes;
	uint32_t		sum;

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
	return ((uint16_t)~sum);
}

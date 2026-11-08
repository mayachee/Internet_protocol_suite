#include "ft_ping.h"

#include <stdio.h>

void print_statistics(unsigned int transmitted, unsigned int received)
{
	unsigned int lost;

	lost = transmitted >= received ? transmitted - received : 0;
	printf("%u packets transmitted, %u packets received, %u lost\n",
		transmitted, received, lost);
}

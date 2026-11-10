#include "ft_ping.h"

void print_statistics(unsigned int transmitted, unsigned int received,
	const double *rtt, size_t rtt_count)
{
	double minimum;
	double maximum;
	double sum;
	double variance;
	double loss;

	loss = transmitted == 0 ? 0.0
		: (double)(transmitted - received) * 100.0 / transmitted;
	printf("%u packets transmitted, %u packets received, %.1f%% packet loss\n",
		transmitted, received, loss);
	if (rtt_count == 0)
		return;
	minimum = rtt[0];
	maximum = rtt[0];
	sum = 0.0;
	for (size_t index = 0; index < rtt_count; index++)
	{
		if (rtt[index] < minimum)
			minimum = rtt[index];
		if (rtt[index] > maximum)
			maximum = rtt[index];
		sum += rtt[index];
	}
	sum /= rtt_count;
	variance = 0.0;
	for (size_t index = 0; index < rtt_count; index++)
		variance += (rtt[index] - sum) * (rtt[index] - sum);
	variance = sqrt(variance / rtt_count);
	printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
		minimum, sum, maximum, variance);
}

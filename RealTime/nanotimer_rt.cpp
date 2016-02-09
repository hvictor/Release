#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "nanotimer_rt.h"

/*
 * Start the nanotimer
 */
void nanotimer_rt_start(struct timespec *start)
{
	clock_gettime(CLOCK_REALTIME, start);
}

/*
 * Stop the nanotimer
 */
void nanotimer_rt_stop(struct timespec *stop)
{
	clock_gettime(CLOCK_REALTIME, stop);
}

/*
 * Compute elapsed time in milliseconds
 */
double nanotimer_rt_ms_diff(struct timespec *start, struct timespec *stop)
{
	return (stop->tv_sec - start->tv_sec) * 1000.0 + (stop->tv_nsec - start->tv_nsec) / 1000000.0;
}
	
/*
 * Compute elapsed time in nanoseconds
 */
double nanotimer_rt_ns_diff(struct timespec *start, struct timespec *stop)
{
	return (stop->tv_sec - start->tv_sec) * 1000000000.0 + (stop->tv_nsec - start->tv_nsec);
}

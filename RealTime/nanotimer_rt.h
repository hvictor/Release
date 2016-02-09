/*
 * nanotimer_rt.h
 *
 *  Created on: Jan 27, 2016
 *      Author: sled
 */

#ifndef NANOTIMER_RT_H_
#define NANOTIMER_RT_H_

#include <time.h>

void nanotimer_rt_start(struct timespec *start);
void nanotimer_rt_stop(struct timespec *stop);
double nanotimer_rt_ms_diff(struct timespec *start, struct timespec *stop);
double nanotimer_rt_ns_diff(struct timespec *start, struct timespec *stop);



#endif /* NANOTIMER_RT_H_ */

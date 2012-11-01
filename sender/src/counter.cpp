/*
 * counter.cpp
 *
 *  Created on: Oct 23, 2012
 *      Author: swebber
 */

#include "counter.h"
#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <sys/time.h>
#include <algorithm>

Counter::Counter(double rate)
{
	// stores the time of day in last_time
	// can access individual seconds and milliseconds by doing last_time->tv_sec or tv_usec
	gettimeofday(&last_time, NULL);

	wait_time.tv_usec = 1000000 * 1/rate;
}

void Counter::wait()
{
	struct timeval curr_time;
	struct timeval time_elapsed;

	gettimeofday(&curr_time, NULL);
	time_elapsed.tv_usec = curr_time.tv_usec - last_time.tv_usec;
	if (wait_time.tv_usec > time_elapsed.tv_usec)
		usleep(wait_time.tv_usec - time_elapsed.tv_usec);

	// wait (time_to_wait - time_elapsed_millisec)
	gettimeofday(&last_time, NULL);
}

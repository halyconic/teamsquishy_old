/*
 * counter.h
 *
 *  Created on: Oct 23, 2012
 *      Author: swebber
 */

#ifndef COUNTER_H_
#define COUNTER_H_

#include <sys/time.h>

class Counter
{
public:
	Counter(double rate);

	void wait();

	struct timeval last_time;
	struct timeval wait_time;
};

#endif /* COUNTER_H_ */

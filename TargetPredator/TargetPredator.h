/*
 * TargetPredator.h
 *
 *  Created on: Apr 28, 2016
 *      Author: sled
 */

#ifndef TARGETPREDATOR_H_
#define TARGETPREDATOR_H_

#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <stdint.h>

using namespace std;

typedef struct
{
	int row;
	int xl;
	int xr;
} pred_scan_t;

class TargetPredator {
public:
	TargetPredator();
	virtual ~TargetPredator();
	pred_scan_t engage_8UC1(uint8_t *data, int width, int height);
};

#endif /* TARGETPREDATOR_H_ */

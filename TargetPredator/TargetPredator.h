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
#include "../Common/opencv_headers.h"

using namespace std;
using namespace cv;

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
	pred_scan_t engage_8UC4(uint8_t *data, int width, int height);
	pred_scan_t engage_Mat8UC1(Mat f_8UC1, int width, int height);
};

#endif /* TARGETPREDATOR_H_ */

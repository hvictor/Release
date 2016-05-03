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
#include <list>
#include <algorithm>
#include "../Common/opencv_headers.h"

using namespace std;
using namespace cv;

typedef struct
{
	int row;
	int xl;
	int xr;
} pred_scan_t;

typedef struct
{
	int x;
	int y;
} pred_state_t;

class TargetPredator {
public:
	TargetPredator();
	virtual ~TargetPredator();
	pred_scan_t engage_8UC1(uint8_t *data, int width, int height);
	pred_scan_t engage_8UC4(uint8_t *data, int width, int height);
	pred_scan_t engage_Mat8UC1(Mat f_8UC1, int width, int height);
	void update_state(int x, int y);
	list<pred_state_t> *get_state();

private:
	list<pred_state_t> state;
};

#endif /* TARGETPREDATOR_H_ */

/*
 * TennisFieldDelimiter.h
 *
 *  Created on: Jan 31, 2016
 *      Author: sled
 */

#ifndef TENNISFIELDDELIMITER_H_
#define TENNISFIELDDELIMITER_H_

#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <vector>

using namespace cv;
using namespace std;


class TennisFieldDelimiter {
public:
	TennisFieldDelimiter();
	virtual ~TennisFieldDelimiter();
	Point2f bottomLeft;
	Point2f bottomRight;
	Point2f topLeft;
	Point2f topRight;
};

#endif /* TENNISFIELDDELIMITER_H_ */

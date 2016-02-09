/*
 * CalibrationWindow.h
 *
 *  Created on: Jan 31, 2016
 *      Author: sled
 */

#ifndef CALIBRATIONWINDOW_H_
#define CALIBRATIONWINDOW_H_

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

class CalibrationWindow {
public:
	CalibrationWindow();
	virtual ~CalibrationWindow();
	bool containsPoint(Point2f p);
	vector<Point> filterPoints(vector<Point> points);
	Point2f bottomLeft;
	Point2f bottomRight;
	Point2f topLeft;
	Point2f topRight;
};

#endif /* CALIBRATIONWINDOW_H_ */

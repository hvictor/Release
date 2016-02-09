/*
 * FlowProcessor.h
 *
 *  Created on: Jan 27, 2016
 *      Author: sled
 */

#ifndef FLOWPROCESSOR_H_
#define FLOWPROCESSOR_H_

#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <time.h>
#include <vector>
#include <algorithm>

#include "../Configuration/configs.h"

using namespace std;
using namespace cv;
using namespace cv::gpu;

typedef struct
{
	unsigned int x;
	unsigned int y;

	Rect rect;
	double displacement_x;
	double displacement_y;
} FlowObject;

vector<FlowObject> FlowProcessor_ProcessSparseFlow(Mat frame0, Mat frame1, vector<Rect> discardedImageAreas);
vector<FlowObject> FlowProcessor_ProcessDenseFlow(const Mat_<float>& flowx, const Mat_<float>& flowy);

#endif /* FLOWPROCESSOR_H_ */

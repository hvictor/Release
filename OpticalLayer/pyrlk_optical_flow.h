/*
 * pyrlk_optical_flow.h
 *
 *  Created on: Jan 27, 2016
 *      Author: sled
 */

#ifndef PYRLK_OPTICAL_FLOW_H_
#define PYRLK_OPTICAL_FLOW_H_

#include <vector>
#include <iterator>
#include <algorithm>
#include <unistd.h>
#include <stdlib.h>
#include "opencv2/core/core.hpp"
#include "opencv2/core/opengl_interop.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/gpu/gpu.hpp"
#include "FlowProcessor.h"

using namespace cv;
using namespace cv::gpu;
using namespace std;

vector<FlowObject> pyrlk_compute(Mat& frame0, Mat& frame1, vector<Rect> discardedImageAreas);

#endif /* PYRLK_OPTICAL_FLOW_H_ */

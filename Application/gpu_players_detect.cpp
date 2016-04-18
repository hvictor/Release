/*
 * gpu_players_detect.cpp
 *
 *  Created on: Jan 24, 2016
 *      Author: sled
 */

#include "gpu_players_detect.h"
#include <iostream>


using namespace std;
using namespace cv;
using namespace cv::gpu;

// Detect players
std::vector<Rect> detectPlayers(Mat& frame0)
{
	gpu::GpuMat src_gpu(frame0);
	gpu::GpuMat d_grayscale;

	if (src_gpu.channels() == 3) {
		gpu::cvtColor(src_gpu, d_grayscale, CV_RGB2GRAY);
	}
	else if (src_gpu.channels() == 4) {
		gpu::cvtColor(src_gpu, d_grayscale, CV_RGBA2GRAY);
	}
	else if (src_gpu.channels() == 1) {
		d_grayscale = src_gpu;
	}

	gpu::HOGDescriptor hog;
	hog.setSVMDetector(cv::gpu::HOGDescriptor::getDefaultPeopleDetector());

	vector<cv::Rect> found;
	hog.detectMultiScale(d_grayscale, found);

	return found;
}



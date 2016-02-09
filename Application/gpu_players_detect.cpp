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
	else if (src_gpu.channels() == 1) {
		d_grayscale = src_gpu;
	}

	gpu::HOGDescriptor hog;
	hog.setSVMDetector(cv::gpu::HOGDescriptor::getDefaultPeopleDetector());

	vector<cv::Rect> found;
	hog.detectMultiScale(d_grayscale, found);

	return found;
}

// Enclose players in a rectangle with an ordered naming
void drawRectAroundPlayers(Mat& frame0, vector<cv::Rect> found)
{
	for(unsigned i = 0; i < found.size(); i++) {
		cv::Rect r = found[i];
		rectangle(frame0, r.tl(), r.br(), Scalar(255, 0, 0), 2);
		putText(frame0, "Player", Point(r.tl().x, r.tl().y - 15), FONT_HERSHEY_SIMPLEX, 0.5, cvScalar(255, 0, 0), 1, CV_AA);
	}
}



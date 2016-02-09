/*
 * GPULineDetector.cpp
 *
 *  Created on: Jan 29, 2016
 *      Author: sled
 */

#include "GPULineDetector.h"



GPULineDetector::GPULineDetector() {
	// TODO Auto-generated constructor stub
	use_probabilistic_detection = true;
	use_cpu = false;
	minLineLength = 0;
	maxLineGap = 0;
	maxLines = 0;
	votesThresh = 0;
}

GPULineDetector::~GPULineDetector() {
	// TODO Auto-generated destructor stub
}

void GPULineDetector::identifyEdgesInFrame(GpuMat& d_inputFrame, GpuMat& d_outputMask)
{
	gpu::Canny(d_inputFrame, d_outputMask, 100, 200, 3);
}

void GPULineDetector::setCUDADetectorParameters(double minLineLength, double maxLineGap, int maxLines, int votesThreshold)
{
	this->minLineLength = minLineLength;
	this->maxLineGap = maxLineGap;
	this->maxLines = maxLines;
	this->votesThresh = votesThreshold;
}

void GPULineDetector::useProbabilisticDetection(bool flag)
{
	use_probabilistic_detection = flag;
}

void GPULineDetector::useCPU(bool flag)
{
	use_cpu = flag;
}

vector<Line> GPULineDetector::detect(GpuMat d_inputFrame)
{
	vector<Line> results;

	GpuMat d_lines;
	GpuMat d_mask;

	identifyEdgesInFrame(d_inputFrame, d_mask);

	printf("Writing mask...\n");
	imwrite("calib/mask.png", Mat(d_mask));

	HoughLinesBuf d_houghLinesBuffer;
	// votes_thresh, min_len, max_points_gap
	if (use_cpu) {
		Mat h_mask(d_mask);
		Mat lines;

		if (use_probabilistic_detection) {
			HoughLinesP(h_mask, lines, 1.0f, (float) (CV_PI / 180.0f), votesThresh, minLineLength, maxLineGap);
		}
		else {
			HoughLines(h_mask, lines, 1.0f, (float) (CV_PI / 180.0f), votesThresh);
		}
	}
	else {
		if (use_probabilistic_detection) {
			printf("CUDA Lines Detector :: Using probabilistic detection\n");
			gpu::HoughLinesP(d_mask, d_lines, d_houghLinesBuffer, 1.0f, (float) (CV_PI / 180.0f), minLineLength, maxLineGap, maxLines);
		}
		else {
			gpu::HoughLines(d_mask, d_lines, d_houghLinesBuffer, 1.0f, (float)(CV_PI / 180.0f), votesThresh);
		}
	}

    vector<Vec4i> gpuLines;

    if (!d_lines.empty()) {
    	gpuLines.resize(d_lines.cols);
    	Mat h_lines(1, d_lines.cols, CV_32SC4, &gpuLines[0]);
    	d_lines.download(h_lines);
    }

    for (size_t i = 0; i < gpuLines.size(); ++i) {
    	Vec4i h_line = gpuLines[i];

    	Line tmp;
    	tmp.p0 = Point(h_line[0], h_line[1]);
    	tmp.p1 = Point(h_line[2], h_line[3]);

    	results.push_back(tmp);
    }

	return results;
}

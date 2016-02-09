/*
 * GPULineDetector.h
 *
 *  Created on: Jan 29, 2016
 *      Author: sled
 */

#ifndef GPULINEDETECTOR_H_
#define GPULINEDETECTOR_H_

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

using namespace std;
using namespace cv;
using namespace cv::gpu;

typedef struct
{
	Point2f p0;
	Point2f p1;
} Line;

class GPULineDetector {
public:
	GPULineDetector();
	virtual ~GPULineDetector();
	vector<Line> detect(GpuMat d_inputFrame);
	void setCUDADetectorParameters(double minLineLength, double maxLineGap, int maxLines, int votesThresh);
	void useCPU(bool flag);
	void useProbabilisticDetection(bool flag);
private:
	void identifyEdgesInFrame(GpuMat& d_inputFrame, GpuMat& d_outputMask);
	double minLineLength;
	double maxLineGap;
	int maxLines;
	int votesThresh;
	bool use_probabilistic_detection;
	bool use_cpu;
};

#endif /* GPULINEDETECTOR_H_ */

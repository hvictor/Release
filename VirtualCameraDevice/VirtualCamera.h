/*
 * VirtualCamera.h
 *
 *  Created on: Jan 29, 2016
 *      Author: sled
 */

#ifndef VIRTUALCAMERA_H_
#define VIRTUALCAMERA_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <opencv2/calib3d/calib3d.hpp>

using namespace std;
using namespace cv;
using namespace cv::gpu;

class VirtualCamera {
public:
	VirtualCamera();
	virtual ~VirtualCamera();
	virtual bool OpenFromVideo(string fileName) = 0;
	virtual bool OpenFromFrameSequence(string frameSequenceDirectoryPath, char *frameFileNameFormat) = 0;
	virtual Mat readFrameFromFrameSequence();
	Mat readFrameFromVideo();
	void rewindFrameSequence();

protected:
	unsigned int frameCounter;
	bool framesDirectorySet;
	VideoCapture videoCapture;
	char frameSequenceFileNameFormat[300];
	char frameSequenceFileNameBuffer[300];

private:
};

#endif /* VIRTUALCAMERA_H_ */

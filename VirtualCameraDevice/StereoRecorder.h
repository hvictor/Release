/*
 * StereoRecorder.h
 *
 *  Created on: Jan 31, 2016
 *      Author: sled
 */

#ifndef STEREORECORDER_H_
#define STEREORECORDER_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include "../Common/opencv_headers.h"

class StereoRecorder {
public:
	StereoRecorder();
	virtual ~StereoRecorder();
	void startFramesRecording(string basePath);
	void stopFramsRecording();
	void record(Mat frameL, Mat frameR);

private:
	string basePath;
	const char *szBasePath;
	int counter;
};

#endif /* STEREORECORDER_H_ */

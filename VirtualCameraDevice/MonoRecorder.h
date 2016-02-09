/*
 * MonoRecorder.h
 *
 *  Created on: Jan 31, 2016
 *      Author: sled
 */

#ifndef MONORECORDER_H_
#define MONORECORDER_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include "../Common/opencv_headers.h"

class MonoRecorder {
public:
	MonoRecorder();
	virtual ~MonoRecorder();
	void startFramesRecording(string basePath);
	void stopFramsRecording();
	void record(Mat frame);

private:
	string basePath;
	const char *szBasePath;
	int counter;
};

#endif /* MONORECORDER_H_ */

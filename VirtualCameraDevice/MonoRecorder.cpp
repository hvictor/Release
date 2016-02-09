/*
 * MonoRecorder.cpp
 *
 *  Created on: Jan 31, 2016
 *      Author: sled
 */

#include "MonoRecorder.h"

MonoRecorder::MonoRecorder()
{
	counter = 0;
	szBasePath = 0;
}

MonoRecorder::~MonoRecorder()
{
}

void MonoRecorder::startFramesRecording(string basePath)
{
	this->basePath = basePath;
	szBasePath = this->basePath.c_str();
}

void MonoRecorder::stopFramsRecording()
{
}

void MonoRecorder::record(Mat frame)
{
	char fileName[600];

	sprintf(fileName, "%s/%04d.png", szBasePath, counter);
	printf("[MONO] Recording: [%s]\n", fileName);
	imwrite(fileName, frame);

	counter++;
}

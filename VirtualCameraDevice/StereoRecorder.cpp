/*
 * StereoRecorder.cpp
 *
 *  Created on: Jan 31, 2016
 *      Author: sled
 */

#include "StereoRecorder.h"

StereoRecorder::StereoRecorder()
{
	counter = 0;
	szBasePath = 0;
}

StereoRecorder::~StereoRecorder()
{
}

void StereoRecorder::startFramesRecording(string basePath)
{
	this->basePath = basePath;
	szBasePath = this->basePath.c_str();
}

void StereoRecorder::stopFramsRecording()
{
}

void StereoRecorder::record(Mat frameL, Mat frameR)
{
	char fileName[600];

	sprintf(fileName, "%s/L/%04d.png", szBasePath, counter);
	printf("[STEREO] Recording: [%s]\n", fileName);
	imwrite(fileName, frameL);
	sprintf(fileName, "%s/R/%04d.png", szBasePath, counter);
	printf("[STEREO] Recording: [%s]\n", fileName);
	imwrite(fileName, frameR);

	counter++;
}

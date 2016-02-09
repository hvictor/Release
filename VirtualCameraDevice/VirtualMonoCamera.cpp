/*
 * VirtualMonoCamera.cpp
 *
 *  Created on: Jan 29, 2016
 *      Author: sled
 */

#include "VirtualMonoCamera.h"

VirtualMonoCamera::VirtualMonoCamera() : VirtualCamera()
{
	grayscaleFlag = (Configuration::getInstance()->getFrameInfo().channels == 1);
}

VirtualMonoCamera::~VirtualMonoCamera() {

}

bool VirtualMonoCamera::OpenFromVideo(string fileName)
{
	return videoCapture.open(fileName);
}

bool VirtualMonoCamera::OpenFromFrameSequence(string frameSequenceDirectoryPath, char *frameFileNameFormat)
{
	sprintf(frameSequenceFileNameFormat, "%s/%s", frameSequenceDirectoryPath.c_str(), frameFileNameFormat);
	framesDirectorySet = true;

	return true;
}

Mat VirtualMonoCamera::readFrameFromFrameSequence()
{
	if (!framesDirectorySet) {
		return Mat();
	}

	sprintf(frameSequenceFileNameBuffer, frameSequenceFileNameFormat, frameCounter++);

	printf("VirtualMonoCamera()::readFrame : reading frame [%s]\n", frameSequenceFileNameBuffer);

	if (grayscaleFlag)
		return imread(frameSequenceFileNameBuffer, CV_LOAD_IMAGE_GRAYSCALE);
	else
		return imread(frameSequenceFileNameBuffer);
}

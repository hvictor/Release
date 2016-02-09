/*
 * VirtualCamera.cpp
 *
 *  Created on: Jan 29, 2016
 *      Author: sled
 */

#include "VirtualCamera.h"

VirtualCamera::VirtualCamera() {
	// TODO Auto-generated constructor stub
	frameCounter = 0;
	framesDirectorySet = false;
}

VirtualCamera::~VirtualCamera() {
	// TODO Auto-generated destructor stub
}

Mat VirtualCamera::readFrameFromVideo()
{
	Mat frame;

	if (videoCapture.isOpened()) {
		videoCapture.read(frame);
	}

	return frame;
}

Mat VirtualCamera::readFrameFromFrameSequence()
{
	return Mat();
}

void VirtualCamera::rewindFrameSequence()
{
	frameCounter = 0;
}

/*
 * VirtualStereoCamera.cpp
 *
 *  Created on: Jan 29, 2016
 *      Author: sled
 */

#include "VirtualStereoCamera.h"
#include "../Serialization/serialization.h"
#include "../Common/filesystem.h"

VirtualStereoCamera::VirtualStereoCamera()
{
	grayscaleFlag = (Configuration::getInstance()->getFrameInfo().channels == 1);
}

VirtualStereoCamera::~VirtualStereoCamera()
{
}

bool VirtualStereoCamera::OpenFromVideo(string fileName)
{
	return videoCapture.open(fileName);
}

bool VirtualStereoCamera::OpenFromFrameSequence(string frameSequenceDirectoryPath, char *frameFileNameFormat)
{
	sprintf(frameSequenceFileNameFormat, "%s/%%c/%s", frameSequenceDirectoryPath.c_str(), frameFileNameFormat);
	framesDirectorySet = true;

	return true;
}

bool VirtualStereoCamera::OpenBinaryRecording(char *fileName)
{
	open_deserialization_channel(fileName);
	return true;
}

void VirtualStereoCamera::CloseBinaryRecording()
{
	close_deserialization_channel();
}

bool VirtualStereoCamera::ReadBinaryRecordingData(FrameData *pFrameData)
{
	printf("VirtualStereoCamera :: Requesting deserialization of binary data\n");

	return deserialize_next_frame_data(pFrameData);
}

FramePair VirtualStereoCamera::readFramePairFromFrameSequence()
{
	FramePair stereoFrame;

	if (!framesDirectorySet) {
		return stereoFrame;
	}

	sprintf(frameSequenceFileNameBuffer, frameSequenceFileNameFormat, 'L', frameCounter);

	if (grayscaleFlag) {
		stereoFrame.L = imread(frameSequenceFileNameBuffer, CV_LOAD_IMAGE_GRAYSCALE);
		sprintf(frameSequenceFileNameBuffer, frameSequenceFileNameFormat, 'R', frameCounter);
		stereoFrame.R = imread(frameSequenceFileNameBuffer, CV_LOAD_IMAGE_GRAYSCALE);
	}
	else {
		stereoFrame.L = imread(frameSequenceFileNameBuffer);
		sprintf(frameSequenceFileNameBuffer, frameSequenceFileNameFormat, 'R', frameCounter);
		stereoFrame.R = imread(frameSequenceFileNameBuffer);
	}

	frameCounter++;

	return stereoFrame;
}

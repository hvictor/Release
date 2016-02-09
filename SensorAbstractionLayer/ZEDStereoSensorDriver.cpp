/*
 * ZEDStereoSensorDriver.cpp
 *
 *  Created on: Feb 1, 2016
 *      Author: sled
 */

#include "ZEDStereoSensorDriver.h"

ZEDStereoSensorDriver::ZEDStereoSensorDriver() {
	frameSize.width = 1280;
	frameSize.height = 720;

	zedProperties = new ZEDCameraProperties();
}

ZEDStereoSensorDriver::~ZEDStereoSensorDriver() {
	delete zedProperties;
}

// Open ZED camera sensor
bool ZEDStereoSensorDriver::openCamera()
{
	// TODO: Not yet implemented

	return true;
}

// Close ZED camera sensor
bool ZEDStereoSensorDriver::closeCamera()
{
	return true; // TODO: Not yet implemented
}

// Get ZED camera properties
ZEDCameraProperties *ZEDStereoSensorDriver::getZEDCameraProperties()
{
	return zedProperties;
}

// Fetch ZED frame
StereoFrame ZEDStereoSensorDriver::fetchStereoFrame()
{
	StereoFrame frame;

	// TODO: Not yet implemented
	//ZEDFrame pFrameData = GetZEDFrame();

	frame.channels = 4;
	frame.bytesLength = frameSize.width * frameSize.height * frame.channels * sizeof(uint8_t);


	return frame;
}


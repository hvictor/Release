/*
 * ZEDStereoSensorDriver.cpp
 *
 *  Created on: Feb 1, 2016
 *      Author: sled
 */

#include "ZEDStereoSensorDriver.h"

ZEDStereoSensorDriver::ZEDStereoSensorDriver() {
	frameSize.width = 640;
	frameSize.height = 480;

	zedProperties = new ZEDCameraProperties();
}

ZEDStereoSensorDriver::~ZEDStereoSensorDriver() {
	delete zedProperties;
}

// Open ZED camera sensor
bool ZEDStereoSensorDriver::openCamera()
{
	this->zed = new sl::zed::Camera(zed::VGA, 30);

	if (zed->init(zed::MODE::PERFORMANCE, 0, true) != zed::SUCCESS) {
		printf("ZED init error\n");
	}

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

	printf("Grabbing...\n");
	zed->grab(zed::SENSING_MODE::FULL);

	printf("Grabbed OK\n");

	frame.channels = 4;
	frame.bytesLength = frameSize.width * frameSize.height * frame.channels * sizeof(uint8_t);

	printf("Assigning data\n");

	frame.leftData = (uint8_t *)(zed->retrieveImage(sl::zed::SIDE::LEFT)).data;
	frame.rightData = (uint8_t *)(zed->retrieveImage(sl::zed::SIDE::RIGHT)).data;

	return frame;
}


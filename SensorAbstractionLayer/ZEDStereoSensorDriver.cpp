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
	this->zed = new sl::zed::Camera(zed::VGA, 30);
}

ZEDStereoSensorDriver::~ZEDStereoSensorDriver() {
	delete zedProperties;
}

// Open ZED camera sensor
bool ZEDStereoSensorDriver::openCamera()
{
	if (zed->init(zed::MODE::PERFORMANCE, 0, true, true) != zed::SUCCESS) {
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
	frame.bytesLength = 0;

	if (this->zed->grab(zed::SENSING_MODE::RAW, true, true)) {
		return frame;
	}

	frame.channels = 4;
	frame.bytesLength = frameSize.width * frameSize.height * frame.channels * sizeof(uint8_t);

	//slMat2cvMat(zed->retrieveImage(sl::zed::SIDE::LEFT)).copyTo(L);
	//slMat2cvMat(zed->retrieveImage(sl::zed::SIDE::RIGHT)).copyTo(R);


	//frame.leftData = (uint8_t *)(zed->retrieveImage(zed::SIDE::LEFT)).data;
	cv::Mat tmp(Size(640, 480), CV_8UC4);
	cv::Mat tmp8u = zed::slMat2cvMat(zed->normalizeMeasure(sl::zed::MEASURE::DEPTH));
	cout << "Type: " << tmp.type() << endl;
	cv::cvtColor(tmp8u, tmp, CV_GRAY2RGBA);
	frame.leftData = (uint8_t *)(tmp.data);
	frame.rightData = (uint8_t *)(zed->retrieveImage(zed::SIDE::RIGHT)).data;


	return frame;
}


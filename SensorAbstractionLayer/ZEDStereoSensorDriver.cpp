/*
 * ZEDStereoSensorDriver.cpp
 *
 *  Created on: Feb 1, 2016
 *      Author: sled
 */

#include "ZEDStereoSensorDriver.h"

ZEDStereoSensorDriver::ZEDStereoSensorDriver()
{
	frameSize.width = 640;
	frameSize.height = 480;

	computeDepth = false;
	computeDisparity = false;
	frameCounter = 0;

	zedProperties = new ZEDCameraProperties();
	this->zed = new sl::zed::Camera(zed::VGA, 30);

	ZEDStereoCameraHardwareParameters zedParam = Configuration::getInstance()->getZEDStereoCameraHardwareParameters();

	depthFrameInterleave = zedParam.depthFrameInterleave;

	switch (zedParam.performanceMode)
	{
	case HighPerformance:
		performanceMode = zed::MODE::PERFORMANCE;
		break;
	case QualityAcquisition:
		performanceMode = zed::MODE::QUALITY;
		break;

	default:
		performanceMode = zed::MODE::PERFORMANCE;
		break;

	}

	switch (zedParam.sensingMode)
	{
	case RawSensing:
		sensingMode = zed::SENSING_MODE::RAW;
		break;
	case FullSensing:
		sensingMode = zed::SENSING_MODE::FULL;
		break;

	default:
		sensingMode = zed::SENSING_MODE::RAW;
		break;

	}
}

ZEDStereoSensorDriver::~ZEDStereoSensorDriver() {
	delete zedProperties;
}

// Open ZED camera sensor
bool ZEDStereoSensorDriver::openCamera()
{
	if (zed->init(performanceMode, 0, true, true) != zed::SUCCESS) {
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

	frameCounter++;

	if (frameCounter >= depthFrameInterleave) {
		computeDepth = true;
		computeDisparity = true;
		frameCounter = 0;
	}

	if (this->zed->grab(sensingMode, computeDisparity, computeDepth)) {
		return frame;
	}

	frame.channels = 4;
	frame.bytesLength = frameSize.width * frameSize.height * frame.channels * sizeof(uint8_t);

	frame.leftData = (uint8_t *)(zed->retrieveImage(zed::SIDE::LEFT)).data;
	frame.rightData = (uint8_t *)(zed->retrieveImage(zed::SIDE::RIGHT)).data;
	frame.depthData = 0;

	if (computeDepth) {
		//frame.depthData = (uint8_t *)(zed->retrieveMeasure(sl::zed::MEASURE::DEPTH)).data;
		zed::Mat depthMat = zed->retrieveMeasure(sl::zed::MEASURE::DEPTH);

		uchar3 val = depthMat.getValue(640/2, 480/2);
		printf("[Driver] matrix: %d, raw_data: %d mm\n", val.c1, depthMat.data[640*(480/2) + 640/2]);

		frame.depthData = (uint8_t *)depthMat.data;
		computeDepth = false;
		computeDisparity = false;
	}

	// DEPTH:
	//cv::Mat tmp8u = zed::slMat2cvMat(zed->normalizeMeasure(sl::zed::MEASURE::DEPTH));
	//frame.leftData = (uint8_t *)(tmp8u.data);



	return frame;
}


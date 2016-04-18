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

	cv::Mat cv_img = zed::slMat2cvMat(zed->retrieveImage(zed::SIDE::LEFT));
	rectangle(cv_img, Point(100-4, 240-4), Point(100+4, 240+4), Scalar(255, 0, 0), 1);
	rectangle(cv_img, Point(500-4, 240-4), Point(500+4, 240+4), Scalar(255, 0, 0), 1);

	frame.leftData = cv_img.data;//(uint8_t *)(zed->retrieveImage(zed::SIDE::LEFT)).data;
	frame.rightData = (uint8_t *)(zed->retrieveImage(zed::SIDE::RIGHT)).data;
	frame.depthData = 0;



	if (computeDepth) {
		/*
		zed::Mat confidence = zed->retrieveMeasure(sl::zed::MEASURE::CONFIDENCE);
		cout << "confidence: channels: " << confidence.channels << ", type: " << confidence.data_type << endl;
		*/

		frame.depthData = (uint8_t *)(zed->retrieveMeasure(sl::zed::MEASURE::DEPTH)).data;
		zed::Mat xyzMat = zed->retrieveMeasure(sl::zed::MEASURE::XYZ);
		int step = xyzMat.step / sizeof(float);

		float *xyz = (float *)xyzMat.data;//zed->retrieveMeasure(sl::zed::MEASURE::XYZ).data;
		float x = xyz[step*(240) + 100 + 0];
		float y = xyz[step*(240) + 100 + 1];
		float z = xyz[step*(240) + 100 + 2];
		printf("[step=%d] X=%.2f, Y=%.2f Z=%.2f\n", step, x, y, z);
		x = xyz[step*(240) + 500 + 0];
		y = xyz[step*(240) + 500 + 1];
		z = xyz[step*(240) + 500 + 2];
		printf("[step=%d] X=%.2f, Y=%.2f Z=%.2f\n", step, x, y, z);
		printf("======\n");


		computeDepth = false;
		computeDisparity = false;
	}

	// DEPTH:
	//cv::Mat tmp8u = zed::slMat2cvMat(zed->normalizeMeasure(sl::zed::MEASURE::DEPTH));
	//frame.leftData = (uint8_t *)(tmp8u.data);



	return frame;
}


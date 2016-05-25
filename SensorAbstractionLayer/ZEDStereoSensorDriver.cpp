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

	camera_open = false;

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

ZEDStereoSensorDriver *ZEDStereoSensorDriver::getInstance()
{
	static ZEDStereoSensorDriver *instance = 0;

	if (!instance) {
		instance = new ZEDStereoSensorDriver();
	}

	return instance;
}

ZEDStereoSensorDriver::~ZEDStereoSensorDriver() {
	delete zedProperties;
}

// Open ZED camera sensor
bool ZEDStereoSensorDriver::openCamera()
{
	if (camera_open)
		return true;

	if (zed->init(performanceMode, 0, true, false, false) != zed::SUCCESS) {
		printf("ZED init error\n");
		camera_open = false;
	}

	camera_open = true;

	return true;
}

bool ZEDStereoSensorDriver::isOpen()
{
	return camera_open;
}

// Close ZED camera sensor
bool ZEDStereoSensorDriver::closeCamera()
{
	if (!camera_open) {
		return true;
	}

	camera_open = false;
	return true; // TODO: Not yet implemented
}

// Get ZED camera properties
ZEDCameraProperties *ZEDStereoSensorDriver::getZEDCameraProperties()
{
	return zedProperties;
}

void ZEDStereoSensorDriver::updateDepthFrameInterleave()
{
	ZEDStereoCameraHardwareParameters zedParam = Configuration::getInstance()->getZEDStereoCameraHardwareParameters();
	depthFrameInterleave = zedParam.depthFrameInterleave;

	printf("ZEDStereoSensorDriver :: Update :: DFI parameter updated to %d [Frames]\n", depthFrameInterleave);
}

// Fetch ZED frame
StereoFrame ZEDStereoSensorDriver::fetchStereoFrame()
{
	StereoFrame frame;
	frame.bytesLength = 0;

	frameCounter++;

	if (frameCounter >= depthFrameInterleave && (depthFrameInterleave > 0)) {
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

		zed::Mat confidence = zed->retrieveMeasure(sl::zed::MEASURE::CONFIDENCE);
		cout << "confidence: channels: " << confidence.channels << ", type: " << confidence.data_type << endl;


		frame.depthData = (uint8_t *)(zed->retrieveMeasure(sl::zed::MEASURE::DEPTH)).data;
		zed::Mat xyzMat = zed->retrieveMeasure(sl::zed::MEASURE::XYZ);
		int step = xyzMat.step / sizeof(float);

		float *xyz = (float *)xyzMat.data;//zed->retrieveMeasure(sl::zed::MEASURE::XYZ).data;
		float x = xyz[step* 320 + 240 + 0];
		float y = xyz[step* 320 + 240 + 1];
		float z = xyz[step* 320 + 240 + 2];

		printf("ZEDStereoSensorDriver :: DEBUG :: [x, y, z] = %.2f, %.2f %.2f, [depth] = %.2f\n", x, y, z, frame.depthData[step * 320 + 240]);


		computeDepth = false;
		computeDisparity = false;
	}

	// DEPTH:
	//cv::Mat tmp8u = zed::slMat2cvMat(zed->normalizeMeasure(sl::zed::MEASURE::DEPTH));
	//frame.leftData = (uint8_t *)(tmp8u.data);



	return frame;
}


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

		this->zed->setConfidenceThreshold(Configuration::getInstance()->dynamicModelParameters.confidenceThreshold);
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

		// Confidence
		zed::Mat confidence = zed->retrieveMeasure(sl::zed::MEASURE::CONFIDENCE);
		frame.confidenceData = (float *)confidence.data;
		frame.stepConfidence = (confidence.step / sizeof(float));

		// Depth
		frame.depthData = (uint8_t *)(zed->retrieveMeasure(sl::zed::MEASURE::DEPTH)).data;

		// XYZ
		zed::Mat xyzMat = zed->retrieveMeasure(sl::zed::MEASURE::XYZ);
		frame.xyzData = (float *)xyzMat.data;
		frame.stepXYZ = (xyzMat.step / sizeof(float));
		frame.xyzMat = new Mat(Size(frameSize.width, frameSize.height), CV_32FC4);
		slMat2cvMat(xyzMat).copyTo(*(frame.xyzMat));

		/*
		float *xyz = (float *)xyzMat.data;//zed->retrieveMeasure(sl::zed::MEASURE::XYZ).data;
		float x = xyz[step* 320 + 240 + 0];
		float y = xyz[step* 320 + 240 + 1];
		float z = xyz[step* 320 + 240 + 2];
		*/

		computeDepth = false;
		computeDisparity = false;
	}

	// DEPTH:
	//cv::Mat tmp8u = zed::slMat2cvMat(zed->normalizeMeasure(sl::zed::MEASURE::DEPTH));
	//frame.leftData = (uint8_t *)(tmp8u.data);



	return frame;
}

int ZEDStereoSensorDriver::retryTargetScan3D(pred_scan_t engage_data, float *xyz_data, int step_xyz, StereoSensorMeasure3D *measurement)
{
	int y = engage_data.row;

	for (int x = engage_data.xl; x <= engage_data.xr; x++) {
		StereoSensorMeasure3D meas = readMeasurementData3D(xyz_data, x, y, step_xyz);

		if (meas.z_mm > 0.0) {
			*measurement = meas;
			return 1;
		}
	}

	return 0;
}

StereoSensorMeasure3D ZEDStereoSensorDriver::readMeasurementData3D(float *data, int x, int y, int step)
{
	StereoSensorMeasure3D meas;

	meas.x_mm = -data[step * y + x] * 0.001;
	meas.y_mm = -data[step * y + x + 1] * 0.001;
	meas.z_mm = data[step * y + x + 2] * 0.001;

	return meas;
}

float ZEDStereoSensorDriver::readMeasurementDataConfidence(float *data, int x, int y, int step)
{
	return data[step * y + x];
}

static StereoSensorMeasure3D readMeasurementMatrix3D(Mat *xyzMat, int x, int y)
{
	StereoSensorMeasure3D meas;
	Vec4f v = xyzMat->at<float>(y, x);

	meas.x_mm = -v.val[0] * 0.001;
	meas.y_mm = -v.val[1] * 0.001;
	meas.z_mm = v.val[2] * 0.001;

	return meas;
}

static int retryTargetScanMatrix3D(pred_scan_t engage_data, Mat *xyzMat, StereoSensorMeasure3D *measurement)
{
	int y = engage_data.row;

	for (int x = engage_data.xl; x <= engage_data.xr; x++) {
		StereoSensorMeasure3D meas = readMeasurementMatrix3D(xyzMat, x, y);

		if (meas.z_mm > 0.0) {
			*measurement = meas;
			return 1;
		}
	}

	return 0;
}

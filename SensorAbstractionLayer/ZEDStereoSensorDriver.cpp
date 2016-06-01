/*
 * ZEDStereoSensorDriver.cpp
 *
 *  Created on: Feb 1, 2016
 *      Author: sled
 */

#include "ZEDStereoSensorDriver.h"
#include "../RealTime/nanotimer_rt.h"

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

	zed::StereoParameters *stereoParam = zed->getParameters();
	_param_L = stereoParam->LeftCam;
	_param_R = stereoParam->RightCam;

	printf("Driver :: FOCAL LEN: x=%.2f, y=%.2f\n", _param_L.fx, _param_L.fy);
	printf("Driver :: OPTICAL CENTER: Cx=%.2f, Cy=%.2f\n", _param_L.cx, _param_L.cy);

	Configuration::getInstance()->zedHardwareParameters.fx_L = _param_L.fx;
	Configuration::getInstance()->zedHardwareParameters.fy_L = _param_L.fy;
	Configuration::getInstance()->zedHardwareParameters.fx_R = _param_R.fx;
	Configuration::getInstance()->zedHardwareParameters.fy_R = _param_R.fy;

	/*
	 * Camera Matrix:
	 *
	 * fx	0	Cx
	 * 0	fy	Cy
	 * 0	0	1
	 *
	 * f: Focal length
	 * C: Optical center of projection
	 */

	Configuration::getInstance()->zedHardwareParameters.cameraMatrix_L.at<float>(0,0) = _param_L.fx;	// fx
	Configuration::getInstance()->zedHardwareParameters.cameraMatrix_L.at<float>(0,1) = 0.0;			// 0
	Configuration::getInstance()->zedHardwareParameters.cameraMatrix_L.at<float>(0,2) = 0.5*_param_L.cx;	// Cx
	Configuration::getInstance()->zedHardwareParameters.cameraMatrix_L.at<float>(1,0) = 0.0;			// 0
	Configuration::getInstance()->zedHardwareParameters.cameraMatrix_L.at<float>(1,1) = _param_L.fy;	// fy
	Configuration::getInstance()->zedHardwareParameters.cameraMatrix_L.at<float>(1,2) = 0.5*_param_L.cy;	// Cy
	Configuration::getInstance()->zedHardwareParameters.cameraMatrix_L.at<float>(2,0) = 0.0;			// 0
	Configuration::getInstance()->zedHardwareParameters.cameraMatrix_L.at<float>(2,1) = 0.0;			// 0
	Configuration::getInstance()->zedHardwareParameters.cameraMatrix_L.at<float>(2,2) = 1.0;			// 1

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

// Triangulate Depth and Image Plane coordinates using reference camera intrinsic
// parameters in order to obtain real-world coordinates.
StereoSensorMeasure3D ZEDStereoSensorDriver::triangulate(float z_depth, float x_image_plane, float y_image_plane)
{
	StereoSensorMeasure3D meas;

	meas.z_mm = z_depth;
	meas.x_mm = (z_depth * x_image_plane) / Configuration::getInstance()->zedHardwareParameters.fx_L;
	meas.y_mm = (z_depth * y_image_plane) / Configuration::getInstance()->zedHardwareParameters.fy_L;

	return meas;
}

// Read Depth data multiple times and mean the value, then compute
float ZEDStereoSensorDriver::repeatedDepthMeasure(int x, int y)
{
	float mean = 0.0;

	int accepted_samples = 0;
	int samples = Configuration::getInstance()->staticModelParameters.groundPlaneModelDepthSamples;

	// Set configured measure confidence threshold
	this->zed->setConfidenceThreshold(Configuration::getInstance()->dynamicModelParameters.confidenceThreshold);

	for (int i = 0; i < samples; i++) {

		// Perform Hardware Measurement
		if (this->zed->grab(zed::SENSING_MODE::RAW, true, true)) {
			// Error, return a mean = 0.0
			return mean;
		}

		// Retrieve Hardware measure
		zed::Mat depthMat = zed->retrieveMeasure(sl::zed::MEASURE::DEPTH);
		float *depth_data = (float *)depthMat.data;
		int step = (depthMat.step / sizeof(float));

		// Read measure from depth data buffer
		float meas = readMeasurementDataDepth(depth_data, x, y, step);

		// Discard invalid measurement
		if (meas <= 0.0) {
			usleep(50000);
			continue;
		}

		mean += meas;
		accepted_samples++;

		printf("ZEDStereoSensorDriver :: Repeated Depth Measure :: P=[%d, %d] Meas=[%d] Depth = %.2f\t[mm]\n", x, y, i, meas);

		usleep(50000);
	}

	mean /= ((float)accepted_samples);
	printf("ZEDStereoSensorDriver :: Repeated Depth Measure :: MEAN Depth = %.2f\t[mm]\n", mean);

	return mean;
}

// Convert

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
		zed::Mat depthMat = zed->retrieveMeasure(sl::zed::MEASURE::DEPTH);
		frame.depthData = (float *)depthMat.data;
		frame.stepDepth = (depthMat.step / sizeof(float));

		// XYZ
		zed::Mat xyzMat = zed->retrieveMeasure(sl::zed::MEASURE::XYZ);
		frame.xyzData = (float *)xyzMat.data;
		frame.stepXYZ = (xyzMat.step / sizeof(float));
		//frame.xyzMat = new Mat(Size(frameSize.width, frameSize.height), CV_32FC4);
		//slMat2cvMat(xyzMat).copyTo(*(frame.xyzMat));

		/*
		float *xyz = (float *)xyzMat.data;//zed->retrieveMeasure(sl::zed::MEASURE::XYZ).data;
		float x = xyz[step* 320 + 240 + 0];
		float y = xyz[step* 320 + 240 + 1];
		float z = xyz[step* 320 + 240 + 2];
		*/

		// Time measurement
		nanotimer_rt_start(&(frame.t));

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

	int meas_count = 0;

	measurement->x_mm = 0.0;
	measurement->y_mm = 0.0;
	measurement->z_mm = 0.0;

	for (int x = engage_data.xl; x <= engage_data.xr; x++) {
		StereoSensorMeasure3D meas = readMeasurementData3D(xyz_data, x, y, step_xyz);

		if (meas.z_mm > 0.0) {
			measurement->x_mm += meas.x_mm;
			measurement->y_mm += meas.y_mm;
			measurement->z_mm += meas.z_mm;

			meas_count++;
		}
	}

	float factor = 1.0 / ((float)meas_count);
	measurement->x_mm *= factor;
	measurement->y_mm *= factor;
	measurement->z_mm *= factor;

	return 0;
}

float ZEDStereoSensorDriver::readMeasurementDataDepth(float *depth_data, int x, int y, int step)
{
	return depth_data[step * y + x];
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

StereoSensorMeasure3D ZEDStereoSensorDriver::readMeasurementMatrix3D(Mat *xyzMat, int x, int y)
{
	StereoSensorMeasure3D meas;
	Vec4f v = xyzMat->at<float>(y, x);

	meas.x_mm = -v.val[0] * 0.001;
	meas.y_mm = -v.val[1] * 0.001;
	meas.z_mm = v.val[2] * 0.001;

	return meas;
}

int ZEDStereoSensorDriver::retryTargetScanMatrix3D(pred_scan_t engage_data, Mat *xyzMat, StereoSensorMeasure3D *measurement)
{
	int y = engage_data.row;

	int meas_count = 0;

	measurement->x_mm = 0.0;
	measurement->y_mm = 0.0;
	measurement->z_mm = 0.0;

	for (int x = engage_data.xl; x <= engage_data.xr; x++) {
		StereoSensorMeasure3D meas = readMeasurementMatrix3D(xyzMat, x, y);

		if (meas.z_mm > 0.0) {
			measurement->x_mm += meas.x_mm;
			measurement->y_mm += meas.y_mm;
			measurement->z_mm += meas.z_mm;

			meas_count++;
		}
	}

	float factor = 1.0 / ((float)meas_count);
	measurement->x_mm *= factor;
	measurement->y_mm *= factor;
	measurement->z_mm *= factor;

	return 0;
}

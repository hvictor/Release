/*
 * DUOStereoSensorDriver.cpp
 *
 *  Created on: Feb 1, 2016
 *      Author: sled
 */

#include "DUOStereoSensorDriver.h"
#include "DUO.h"

DUOStereoSensorDriver::DUOStereoSensorDriver(DUOStereoCameraHardwareParameters duoHardwareParam)
{
	frameSize.width = 320;
	frameSize.height = 240;

	duoProperties = new DUOCameraProperties();
	duoProperties->gain = duoHardwareParam.gain;
	duoProperties->exposure = duoHardwareParam.exposure;
	duoProperties->led = duoHardwareParam.leds;
}

DUOStereoSensorDriver::~DUOStereoSensorDriver() {
	delete duoProperties;
}

bool DUOStereoSensorDriver::isOpen()
{

}

// Open DUO camera sensor
bool DUOStereoSensorDriver::openCamera()
{
	if (camera_open)
		return true;

	if(!OpenDUOCamera(frameSize.width, frameSize.height, 30))
	{
		camera_open = false;
		return false;
	}

	camera_open = true;

	// Set exposure and LED brightness
	SetGain(duoProperties->gain);
	SetExposure(duoProperties->exposure);
	SetLed(duoProperties->led);

	// Un-flip frames
	SetVFlip(true);

	// Enable retrieval of undistorted (rectified) frames
	SetUndistort(true);

	return true;
}

// Close DUO camera sensor
bool DUOStereoSensorDriver::closeCamera()
{
	return true; // TODO: Not yet implemented
}

// Get DUO camera properties
DUOCameraProperties *DUOStereoSensorDriver::getDUOCameraProperties()
{
	return duoProperties;
}

// Fetch DUO frame
StereoFrame DUOStereoSensorDriver::fetchStereoFrame()
{
	StereoFrame frame;

	PDUOFrame pFrameData = GetDUOFrame();

	frame.channels = 1;
	frame.bytesLength = frameSize.width * frameSize.height * frame.channels * sizeof(uint8_t);
	frame.leftData = (uint8_t *)pFrameData->leftData;
	frame.rightData = (uint8_t *)pFrameData->rightData;

	return frame;
}

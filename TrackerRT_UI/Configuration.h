/*
 * Configuration.h
 *
 *  Created on: Feb 1, 2016
 *      Author: sled
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <time.h>
#include <string>

using namespace std;
using namespace cv;

enum DepthSensorTechnology
{
	Stereo,
	IR
};

enum InputSensorDevice
{
	StereoCameraDUO,		// Use DUO stereo camera sensor
	StereoCameraZED,		// Use ZED stereo camera sensor
	StereoCameraVirtual, 	// Use Virtualized stereo camera
	MonoCameraVirtual,		// Use Virtualized mono camera
	IrCameraKinect			// Use Microsoft Kinect
};

enum ProcessingMode
{
	Record,					// Record input frames
	Tracking				// Process input frames for Tracking
};

enum OutputFrameType
{
	Immuted,				// The output frame is of the same type as the input
	ForcedRGB				// Force RGB output frames for Overlay visual effects
};

typedef struct
{
	// Input Device
	InputSensorDevice inputDevice;

	// Processing
	ProcessingMode processingMode;
} OperationalMode;

typedef struct
{
	// Linear Low-Pass filter parameters
	double linearLowPassFilterX;
	double linearLowPassFilterY;

	// Stateful Object Filter (SOF) parameter
	double lucasKanadeOpticalFlowMinMotion;
	double statefulObjectFilterRelatedMaxDiffSphereX;
	double statefulObjectFilterRelatedMaxDiffSphereY;
	double statefulObjectFilterRelatedMaxDiffDirectionalX;
	double statefulObjectFilterRelatedMaxDiffDirectionalY;
	int statefulObjectFilterIdleMaxTicks;
	int statefulObjectFilterTrajectoryMinStates;

} OpticalLayerParameters;

typedef struct
{
	int gain;
	int exposure;
	int leds;
} DUOStereoCameraHardwareParameters;

typedef struct
{
	int width;
	int height;
	int channels;
	OutputFrameType outputType;
} FrameInfo;

typedef struct
{
	double extremaNeighbourhoodSize;
} InterpolationEngineParameters;

class Configuration {
public:

	// Set configuration components
	static Configuration *getInstance();
	void setOperationalMode(InputSensorDevice inputDevice, ProcessingMode processingMode);
	void setOpticalLayerParameters(OpticalLayerParameters opticalLayerParams);
	void setDUOStereoCameraHardwareParameters(DUOStereoCameraHardwareParameters duoParam);
	void setFrameInfo(FrameInfo frameInfo);
	void setInterpolationEngineParameters(InterpolationEngineParameters interpEngineParam);

	// Get configuration components
	FrameInfo getFrameInfo();
	OperationalMode getOperationalMode();
	OpticalLayerParameters getOpticalLayerParameters();
	DUOStereoCameraHardwareParameters getDUOStereoCameraHardwareParameters();
	InterpolationEngineParameters getInterpolationEngineParameters();

	void writeConfigFile(string fileName);
	void loadConfigFile(string fileName);
	void display();
	virtual ~Configuration();

private:
	Configuration();
	FrameInfo frameInfo;
	OperationalMode operationalMode;
	OpticalLayerParameters opticalLayerParameters;
	DUOStereoCameraHardwareParameters duoHardwareParameters;
	InterpolationEngineParameters interpolationEngineParameters;
};

#endif /* CONFIGURATION_H_ */
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../OpticalLayer/HSVManager.h"
#include "../Common/opencv_headers.h"

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

enum ZEDPerformanceMode
{
	HighPerformance,
	QualityAcquisition
};

enum ZEDSensingMode
{
	RawSensing,
	FullSensing
};

enum TrackingWindowMode
{
	AdaptiveTrackingWindow,
	StaticTrackingWindow
};

enum PlayLogicType
{
	TwoPlayers,
	SinglePlayer
};

typedef struct
{
	// Input Device
	InputSensorDevice inputDevice;

	// Processing
	ProcessingMode processingMode;

	// Record Dynamic Models Data
	bool recordDynamicModelsData;
} OperationalMode;

typedef struct
{
	// Frame buffer size
	int frameBufferSize;

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
	ZEDPerformanceMode performanceMode;
	ZEDSensingMode sensingMode;

	int performanceModeIntValue;
	int sensingModeIntValue;

	int depthFrameInterleave;

	// Focal Lengths
	float fx_L;
	float fy_L;
	float fx_R;
	float fy_R;

	// Reference Camera Matrix
	Mat cameraMatrix_L;

	// Reference Camera Rotation and Translation vectors
	Mat rotationVector_L;
	Mat translationVector_L;

} ZEDStereoCameraHardwareParameters;

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

typedef struct
{
	HSVRange targetHSVRange;
	HSVRange fieldMarkersHSVRange;
} CalibrationData;

typedef struct
{
	int linesSensitivityEPS;
	int groundPlaneModelDepthSamples;
	double netHeight;
} StaticModelParameters;

typedef struct
{
	bool freePlay;
	bool trackingWndEnabled;
	bool visualizeTrackingWnd;
	TrackingWindowMode trackingWndMode;
	int trackingWndSize;
	double trackingWndAdaptiveFactorX;
	double trackingWndAdaptiveFactorY;
	int confidenceThreshold;

	// 2D Impact velocity vector angle thresh
	double impact2DVelocityVectorAngleThresh;

	// Dynamic Model 3D
	bool recalcDynamicModel3D;
	bool useInputKalmanFilter;
	double impactMaxFloorDistance;

	// Target Predator
	bool notifyTGTLostToModel;
	int targetPredatorTGTLOSTFramesThreshold;
} DynamicModelParameters;

typedef struct
{
	char recordingDirectory[100];
	char recordingFileName[80];
	char recordingFileNameFullPath[180];
} RecordingParameters;

typedef struct
{
	PlayLogicType playLogicType;
} PlayLogicParameters;

class Configuration {
public:

	// Set configuration components
	static Configuration *getInstance();
	void setOperationalMode(InputSensorDevice inputDevice, ProcessingMode processingMode);
	void setOpticalLayerParameters(OpticalLayerParameters opticalLayerParams);
	void setDUOStereoCameraHardwareParameters(DUOStereoCameraHardwareParameters duoParam);
	void setZEDStereoCameraHardwareParameters(ZEDStereoCameraHardwareParameters zedParam);
	void setFrameInfo(FrameInfo frameInfo);
	void setInterpolationEngineParameters(InterpolationEngineParameters interpEngineParam);
	void setStaticModelLinesSensitivityEPS(int value);
	StaticModelParameters getStaticModelParameters();
	PlayLogicParameters playLogicParameters;
	RecordingParameters recordingParameters;
	void publish();

	// Public properties
	OperationalMode operationalMode;
	OpticalLayerParameters opticalLayerParameters;
	StaticModelParameters staticModelParameters;
	DynamicModelParameters dynamicModelParameters;
	CalibrationData calibrationData;
	ZEDStereoCameraHardwareParameters zedHardwareParameters;

	// Get configuration components
	FrameInfo getFrameInfo();
	OperationalMode getOperationalMode();
	OpticalLayerParameters getOpticalLayerParameters();
	DUOStereoCameraHardwareParameters getDUOStereoCameraHardwareParameters();
	ZEDStereoCameraHardwareParameters getZEDStereoCameraHardwareParameters();
	InterpolationEngineParameters getInterpolationEngineParameters();

	void writeConfigFile(string fileName);
	void loadConfigFile(string fileName);
	void display();
	virtual ~Configuration();

private:
	Configuration();
	FrameInfo frameInfo;
	DUOStereoCameraHardwareParameters duoHardwareParameters;
	InterpolationEngineParameters interpolationEngineParameters;
};

#endif /* CONFIGURATION_H_ */

/*
 * Configuration.cpp
 *
 *  Created on: Feb 1, 2016
 *      Author: sled
 */

#include "Configuration.h"

#include <zed/Camera.hpp>
#include <zed/utils/GlobalDefine.hpp>

#include "../Common/filesystem.h"
#include "../SensorAbstractionLayer/ZEDStereoSensorDriver.h"
#include "../TargetPredator/TargetPredator.h"

using namespace sl;

Configuration *Configuration::getInstance()
{
	static Configuration *instance = 0;

	if (!instance) {
		instance = new Configuration();
	}

	return instance;
}

Configuration::Configuration()
{
	operationalMode.recordDynamicModelsData = false;

	staticModelParameters.groundPlaneModelDepthSamples = 5;
	staticModelParameters.netHeight = 500.0;

	dynamicModelParameters.freePlay = false;
	dynamicModelParameters.trackingWndEnabled = false;
	dynamicModelParameters.confidenceThreshold = 100;
	dynamicModelParameters.trackingWndEnabled = false;
	dynamicModelParameters.useInputKalmanFilter = false;
	dynamicModelParameters.impactMaxFloorDistance = 350.0;

	zedHardwareParameters.cameraMatrix_L = Mat(3, 3, CV_32FC1);
	zedHardwareParameters.rotationVector_L = Mat(1, 3, CV_32FC1);
	zedHardwareParameters.translationVector_L = Mat(1, 3, CV_32FC1);

	for (int l = 0; l < 3; l++) { zedHardwareParameters.rotationVector_L.at<float>(0, l) = 0.0; }
	for (int l = 0; l < 3; l++) { zedHardwareParameters.translationVector_L.at<float>(0, l) = 0.0; }

	playLogicParameters.playLogicType = TwoPlayers;

	dynamicModelParameters.notifyTGTLostToModel = false;
	dynamicModelParameters.targetPredatorTGTLOSTFramesThreshold = 30;

	opticalLayerParameters.linearLowPassFilterX = 1.0;
	opticalLayerParameters.linearLowPassFilterY = 1.0;

	strcpy(recordingParameters.recordingDirectory, "/tmp/recordings/");
	strcpy(recordingParameters.recordingFileName, "tracker.bin");
	sprintf(recordingParameters.recordingFileNameFullPath, "%s%s", recordingParameters.recordingDirectory, recordingParameters.recordingFileName);

	filesystem_make_directory(recordingParameters.recordingDirectory);

	dynamicModelParameters.impact2DVelocityVectorAngleThresh = 30.0;
}

Configuration::~Configuration() {
}

void Configuration::publish()
{
	ZEDStereoSensorDriver::getInstance()->updateDepthFrameInterleave();
	TargetPredator::getInstance()->update_free_play_param();
}

void Configuration::setOperationalMode(InputSensorDevice inputDevice, ProcessingMode processingMode)
{
	operationalMode.inputDevice = inputDevice;
	operationalMode.processingMode = processingMode;
}

void Configuration::setStaticModelLinesSensitivityEPS(int value)
{
	staticModelParameters.linesSensitivityEPS = value;
}

StaticModelParameters Configuration::getStaticModelParameters()
{
	return staticModelParameters;
}

void Configuration::setOpticalLayerParameters(OpticalLayerParameters opticalLayerParam)
{
	opticalLayerParameters = opticalLayerParam;
}

OperationalMode Configuration::getOperationalMode()
{
	return operationalMode;
}

OpticalLayerParameters Configuration::getOpticalLayerParameters()
{
	return opticalLayerParameters;
}

DUOStereoCameraHardwareParameters Configuration::getDUOStereoCameraHardwareParameters()
{
	return duoHardwareParameters;
}

void Configuration::setDUOStereoCameraHardwareParameters(DUOStereoCameraHardwareParameters duoParam)
{
	duoHardwareParameters = duoParam;
}

ZEDStereoCameraHardwareParameters Configuration::getZEDStereoCameraHardwareParameters()
{
	return zedHardwareParameters;
}

void Configuration::setZEDStereoCameraHardwareParameters(ZEDStereoCameraHardwareParameters zedParam)
{
	zedHardwareParameters = zedParam;
}

void Configuration::setFrameInfo(FrameInfo frameInfo)
{
	this->frameInfo = frameInfo;
}

FrameInfo Configuration::getFrameInfo()
{
	return frameInfo;
}

void Configuration::setInterpolationEngineParameters(InterpolationEngineParameters interpEngineParam)
{
	interpolationEngineParameters = interpEngineParam;
}

InterpolationEngineParameters Configuration::getInterpolationEngineParameters()
{
	return interpolationEngineParameters;
}

void Configuration::writeConfigFile(string fileName)
{
	FileStorage fs(".xml", FileStorage::WRITE + FileStorage::MEMORY);

	// Frame information
	fs << "Frame_Width" << frameInfo.width;
	fs << "Frame_Height" << frameInfo.height;
	fs << "Frame_Channels" << frameInfo.channels;
	fs << "Frame_OutputType" << frameInfo.outputType;

	// Operational Mode
	fs << "OperationalMode_InputDevice" << operationalMode.inputDevice;
	fs << "OperationalMode_ProcessingMode" << operationalMode.processingMode;

	// Optical Layer
	fs << "OpticalLayer_FrameBufferSize" << opticalLayerParameters.frameBufferSize;
	fs << "OpticalLayer_LinearLowPassFilterX" << opticalLayerParameters.linearLowPassFilterX;
	fs << "OpticalLayer_LinearLowPassFilterY" << opticalLayerParameters.linearLowPassFilterY;
	fs << "OpticalLayer_LucasKanadeOpticalFlowMinMotion" << opticalLayerParameters.lucasKanadeOpticalFlowMinMotion;
	fs << "OpticalLayer_StatefulObjectFilterRelatedMaxDiffSphereX" << opticalLayerParameters.statefulObjectFilterRelatedMaxDiffSphereX;
	fs << "OpticalLayer_StatefulObjectFilterRelatedMaxDiffSphereY" << opticalLayerParameters.statefulObjectFilterRelatedMaxDiffSphereY;
	fs << "OpticalLayer_StatefulObjectFilterRelatedMaxDiffDirectionalX" << opticalLayerParameters.statefulObjectFilterRelatedMaxDiffDirectionalX;
	fs << "OpticalLayer_StatefulObjectFilterRelatedMaxDiffDirectionalY" << opticalLayerParameters.statefulObjectFilterRelatedMaxDiffDirectionalY;
	fs << "OpticalLayer_StatefulObjectFilterIdleMaxTicks" << opticalLayerParameters.statefulObjectFilterIdleMaxTicks;
	fs << "OpticalLayer_StatefulObjectFilterTrajectoryMinStates" << opticalLayerParameters.statefulObjectFilterTrajectoryMinStates;

	// DUO Stereo Camera
	fs << "StereoCameraDUO_Gain" << duoHardwareParameters.gain;
	fs << "StereoCameraDUO_Exposure" << duoHardwareParameters.exposure;
	fs << "StereoCameraDUO_Leds" << duoHardwareParameters.leds;

	// ZED Stereo Camera
	fs << "StereoCameraZED_PerformanceMode" << zedHardwareParameters.performanceMode;
	fs << "StereoCameraZED_SensingMode" << zedHardwareParameters.sensingMode;
	fs << "StereoCameraZED_DepthFrameInterleave" << zedHardwareParameters.depthFrameInterleave;

	// Interpolation Engine
	fs << "InterpolationEngine_ExtremaNeighbourhoodSize" << interpolationEngineParameters.extremaNeighbourhoodSize;

	// Calibration Data
	fs << "CalibrationData_TGT_Hmin" << (int)calibrationData.targetHSVRange.Hmin;
	fs << "CalibrationData_TGT_Smin" << (int)calibrationData.targetHSVRange.Smin;
	fs << "CalibrationData_TGT_Vmin" << (int)calibrationData.targetHSVRange.Vmin;
	fs << "CalibrationData_TGT_Hmax" << (int)calibrationData.targetHSVRange.Hmax;
	fs << "CalibrationData_TGT_Smax" << (int)calibrationData.targetHSVRange.Smax;
	fs << "CalibrationData_TGT_Vmax" << (int)calibrationData.targetHSVRange.Vmax;

	fs << "CalibrationData_FLD_2D_NearL_X" << TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomLeft.x;
	fs << "CalibrationData_FLD_2D_NearL_Y" << TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomLeft.y;
	fs << "CalibrationData_FLD_2D_NearR_X" << TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomRight.x;
	fs << "CalibrationData_FLD_2D_NearR_Y" << TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomRight.y;
	fs << "CalibrationData_FLD_2D_FarR_X" << TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topRight.x;
	fs << "CalibrationData_FLD_2D_FarR_Y" << TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topRight.y;
	fs << "CalibrationData_FLD_2D_FarL_X" << TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topLeft.x;
	fs << "CalibrationData_FLD_2D_FarL_Y" << TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topLeft.y;

	fs << "StaticModel_Net_2D_BaseNearX" << NetModel::getInstance()->getNetVisualProjection().baseNear.x;
	fs << "StaticModel_Net_2D_BaseNearY" << NetModel::getInstance()->getNetVisualProjection().baseNear.y;
	fs << "StaticModel_Net_2D_TopNearX" << NetModel::getInstance()->getNetVisualProjection().topNear.x;
	fs << "StaticModel_Net_2D_TopNearY" << NetModel::getInstance()->getNetVisualProjection().topNear.y;
	fs << "StaticModel_Net_2D_TopFarX" << NetModel::getInstance()->getNetVisualProjection().topFar.x;
	fs << "StaticModel_Net_2D_TopFarY" << NetModel::getInstance()->getNetVisualProjection().topFar.y;
	fs << "StaticModel_Net_2D_BaseFarX" << NetModel::getInstance()->getNetVisualProjection().baseFar.x;
	fs << "StaticModel_Net_2D_BaseFarY" << NetModel::getInstance()->getNetVisualProjection().baseFar.y;

	fs << "StaticModel_Net_3D_BaseNearX" << NetModel::getInstance()->getNetCoordinates3D().baseNear.x;
	fs << "StaticModel_Net_3D_BaseNearY" << NetModel::getInstance()->getNetCoordinates3D().baseNear.y;
	fs << "StaticModel_Net_3D_BaseNearz" << NetModel::getInstance()->getNetCoordinates3D().baseNear.z;
	fs << "StaticModel_Net_3D_TopNearX" << NetModel::getInstance()->getNetCoordinates3D().topNear.x;
	fs << "StaticModel_Net_3D_TopNearY" << NetModel::getInstance()->getNetCoordinates3D().topNear.y;
	fs << "StaticModel_Net_3D_TopNearZ" << NetModel::getInstance()->getNetCoordinates3D().topNear.z;
	fs << "StaticModel_Net_3D_TopFarX" << NetModel::getInstance()->getNetCoordinates3D().topFar.x;
	fs << "StaticModel_Net_3D_TopFarY" << NetModel::getInstance()->getNetCoordinates3D().topFar.y;
	fs << "StaticModel_Net_3D_TopFarZ" << NetModel::getInstance()->getNetCoordinates3D().topFar.z;
	fs << "StaticModel_Net_3D_BaseFarX" << NetModel::getInstance()->getNetCoordinates3D().baseFar.x;
	fs << "StaticModel_Net_3D_BaseFarY" << NetModel::getInstance()->getNetCoordinates3D().baseFar.y;
	fs << "StaticModel_Net_3D_BaseFarZ" << NetModel::getInstance()->getNetCoordinates3D().baseFar.z;

	fs << "StaticModel_GroundPlaneLinearModelFactorX" << GroundModel::getInstance()->getGroundPlaneLinearModel().a;
	fs << "StaticModel_GroundPlaneLinearModelFactorY" << GroundModel::getInstance()->getGroundPlaneLinearModel().b;
	fs << "StaticModel_GroundPlaneLinearModelFactorZ" << GroundModel::getInstance()->getGroundPlaneLinearModel().c;
	fs << "StaticModel_GroundPlaneLinearModelFactorD" << GroundModel::getInstance()->getGroundPlaneLinearModel().d;

	string buf = fs.releaseAndGetString();

	FILE *fp = fopen(fileName.c_str(), "w");
	fprintf(fp, "%s", buf.c_str());
	fclose(fp);
}

void Configuration::loadConfigFile(string fileName)
{
	std::ifstream t(fileName.c_str());
	std::stringstream buffer;
	buffer << t.rdbuf();
	string s = buffer.str();

	FileStorage fs(s, FileStorage::READ + FileStorage::MEMORY);

	// Temporary aux variables
	int iTmp;
	double dTmp;

	// Frame configuration
	fs["Frame_Width"] >> frameInfo.width;
	fs["Frame_Height"] >> frameInfo.height;
	fs["Frame_Channels"] >> frameInfo.channels;
	fs["Frame_OutputType"] >> iTmp;
	frameInfo.outputType = static_cast<OutputFrameType>(iTmp);

	// Operational Mode
	fs["OperationalMode_InputDevice"] >> iTmp;
	operationalMode.inputDevice = static_cast<InputSensorDevice>(iTmp);
	fs["OperationalMode_ProcessingMode"] >> iTmp;
	operationalMode.processingMode = static_cast<ProcessingMode>(iTmp);

	// Optical Layer
	fs["OpticalLayer_FrameBufferSize"] >> opticalLayerParameters.frameBufferSize;
	fs["OpticalLayer_LinearLowPassFilterX"] >> dTmp;
	opticalLayerParameters.linearLowPassFilterX = dTmp;
	fs["OpticalLayer_LinearLowPassFilterY"] >> dTmp;
	opticalLayerParameters.linearLowPassFilterY = dTmp;
	fs["OpticalLayer_LucasKanadeOpticalFlowMinMotion"] >> opticalLayerParameters.lucasKanadeOpticalFlowMinMotion;
	fs["OpticalLayer_StatefulObjectFilterRelatedMaxDiffSphereX"] >> opticalLayerParameters.statefulObjectFilterRelatedMaxDiffSphereX;
	fs["OpticalLayer_StatefulObjectFilterRelatedMaxDiffSphereY"] >> opticalLayerParameters.statefulObjectFilterRelatedMaxDiffSphereY;
	fs["OpticalLayer_StatefulObjectFilterRelatedMaxDiffDirectionalX"] >> opticalLayerParameters.statefulObjectFilterRelatedMaxDiffDirectionalX;
	fs["OpticalLayer_StatefulObjectFilterRelatedMaxDiffDirectionalY"] >> opticalLayerParameters.statefulObjectFilterRelatedMaxDiffDirectionalY;
	fs["OpticalLayer_StatefulObjectFilterIdleMaxTicks"] >> opticalLayerParameters.statefulObjectFilterIdleMaxTicks;
	fs["OpticalLayer_StatefulObjectFilterTrajectoryMinStates"] >> opticalLayerParameters.statefulObjectFilterTrajectoryMinStates;

	// DUO Stereo Camera
	fs["StereoCameraDUO_Gain"] >> duoHardwareParameters.gain;
	fs["StereoCameraDUO_Exposure"] >> duoHardwareParameters.exposure;
	fs["StereoCameraDUO_Leds"] >> duoHardwareParameters.leds;

	// ZED Stereo Camera
	fs["StereoCameraZED_PerformanceMode"] >> iTmp;
	zedHardwareParameters.performanceMode = static_cast<ZEDPerformanceMode>(iTmp);
	fs["StereoCameraZED_SensingMode"] >> iTmp;
	zedHardwareParameters.sensingMode = static_cast<ZEDSensingMode>(iTmp);
	fs["StereoCameraZED_DepthFrameInterleave"] >> zedHardwareParameters.depthFrameInterleave;

	// Interpolation Engine
	fs["InterpolationEngine_ExtremaNeighbourhoodSize"] >> interpolationEngineParameters.extremaNeighbourhoodSize;

	// Calibration Data
	fs["CalibrationData_TGT_Hmin"] >> calibrationData.targetHSVRange.Hmin;
	fs["CalibrationData_TGT_Smin"] >> calibrationData.targetHSVRange.Smin;
	fs["CalibrationData_TGT_Vmin"] >> calibrationData.targetHSVRange.Vmin;
	fs["CalibrationData_TGT_Hmax"] >> calibrationData.targetHSVRange.Hmax;
	fs["CalibrationData_TGT_Smax"] >> calibrationData.targetHSVRange.Smax;
	fs["CalibrationData_TGT_Vmax"] >> calibrationData.targetHSVRange.Vmax;

	fs["CalibrationData_FLD_2D_NearL_X"] >> TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomLeft.x;
	fs["CalibrationData_FLD_2D_NearL_Y"] >> TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomLeft.y;
	fs["CalibrationData_FLD_2D_NearR_X"] >> TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomRight.x;
	fs["CalibrationData_FLD_2D_NearR_Y"] >> TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomRight.y;
	fs["CalibrationData_FLD_2D_FarR_X"] >> TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topRight.x;
	fs["CalibrationData_FLD_2D_FarR_Y"] >> TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topRight.y;
	fs["CalibrationData_FLD_2D_FarL_X"] >> TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topLeft.x;
	fs["CalibrationData_FLD_2D_FarL_Y"] >> TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topLeft.y;

	// Static Model Data
	NetVisualProjection tmpNetVisualProjection;
	fs["StaticModel_Net_2D_BaseNearX"] >> tmpNetVisualProjection.baseNear.x;
	fs["StaticModel_Net_2D_BaseNearY"] >> tmpNetVisualProjection.baseNear.y;
	fs["StaticModel_Net_2D_TopNearX"] >> tmpNetVisualProjection.topNear.x;
	fs["StaticModel_Net_2D_TopNearY"] >> tmpNetVisualProjection.topNear.y;
	fs["StaticModel_Net_2D_TopFarX"] >> tmpNetVisualProjection.topFar.x;
	fs["StaticModel_Net_2D_TopFarY"] >> tmpNetVisualProjection.topFar.y;
	fs["StaticModel_Net_2D_BaseFarX"] >> tmpNetVisualProjection.baseFar.x;
	fs["StaticModel_Net_2D_BaseFarY"] >> tmpNetVisualProjection.baseFar.y;
	NetModel::getInstance()->setNetVisualProjection(tmpNetVisualProjection);

	NetCoordinates3D tmpNetCoord3D;
	fs["StaticModel_Net_3D_BaseNearX"] >> tmpNetCoord3D.baseNear.x;
	fs["StaticModel_Net_3D_BaseNearY"] >> tmpNetCoord3D.baseNear.y;
	fs["StaticModel_Net_3D_BaseNearz"] >> tmpNetCoord3D.baseNear.z;
	fs["StaticModel_Net_3D_TopNearX"] >> tmpNetCoord3D.topNear.x;
	fs["StaticModel_Net_3D_TopNearY"] >> tmpNetCoord3D.topNear.y;
	fs["StaticModel_Net_3D_TopNearZ"] >> tmpNetCoord3D.topNear.z;
	fs["StaticModel_Net_3D_TopFarX"] >> tmpNetCoord3D.topFar.x;
	fs["StaticModel_Net_3D_TopFarY"] >> tmpNetCoord3D.topFar.y;
	fs["StaticModel_Net_3D_TopFarZ"] >> tmpNetCoord3D.topFar.z;
	fs["StaticModel_Net_3D_BaseFarX"] >> tmpNetCoord3D.baseFar.x;
	fs["StaticModel_Net_3D_BaseFarY"] >> tmpNetCoord3D.baseFar.y;
	fs["StaticModel_Net_3D_BaseFarZ"] >> tmpNetCoord3D.baseFar.z;
	NetModel::getInstance()->setNetCoordinates3D(tmpNetCoord3D);

	double a, b, c, d;
	fs["StaticModel_GroundPlaneLinearModelFactorX"] >> a;
	fs["StaticModel_GroundPlaneLinearModelFactorY"] >> b;
	fs["StaticModel_GroundPlaneLinearModelFactorZ"] >> c;
	fs["StaticModel_GroundPlaneLinearModelFactorD"] >> d;
	GroundModel::getInstance()->setGroundPlaneLinearModelFactorX(a);
	GroundModel::getInstance()->setGroundPlaneLinearModelFactorY(b);
	GroundModel::getInstance()->setGroundPlaneLinearModelFactorZ(c);
	GroundModel::getInstance()->setGroundPlaneLinearModelFactorD(d);
}

void Configuration::display()
{
	cout << "-------------------------------------------" << endl;
	cout << "System Configuration" << endl;
	cout << "-------------------------------------------" << endl;
	cout << endl;
	cout << "[Frame Info]" << endl;
	cout << "\tFrame Width:\t\t" << frameInfo.width << endl;
	cout << "\tFrame Height:\t\t" << frameInfo.height << endl;
	cout << "\tFrame Channels:\t\t" << frameInfo.channels << endl;
	cout << "\tFrame Output Type:\t\t" << frameInfo.outputType << endl;
	cout << "[Operational Mode]" << endl;
	cout << "\tInput Device:\t\t" << operationalMode.inputDevice << endl;
	cout << "\tProcessing Mode:\t\t" << operationalMode.processingMode << endl;
	cout << "[Optical Layer]" << endl;
	cout << "\tFrame Buffer Size:\t\t" << opticalLayerParameters.frameBufferSize << endl;
	cout << "\tLinear Low-Pass Filter X:\t\t" << opticalLayerParameters.linearLowPassFilterX << endl;
	cout << "\tLinear Low-Pass Filter Y:\t\t" << opticalLayerParameters.linearLowPassFilterY << endl;
	cout << "\tOpticalLayer_StatefulObjectFilterRelatedMaxDiffSphereX:\t" << opticalLayerParameters.statefulObjectFilterRelatedMaxDiffSphereX << endl;
	cout << "\tOpticalLayer_StatefulObjectFilterRelatedMaxDiffSphereY:\t" << opticalLayerParameters.statefulObjectFilterRelatedMaxDiffSphereY << endl;
	cout << "\tOpticalLayer_StatefulObjectFilterRelatedMaxDiffDirectionalX:\t" << opticalLayerParameters.statefulObjectFilterRelatedMaxDiffDirectionalX << endl;
	cout << "\tOpticalLayer_StatefulObjectFilterRelatedMaxDiffDirectionalY:\t" << opticalLayerParameters.statefulObjectFilterRelatedMaxDiffDirectionalY << endl;
	cout << "\tOpticalLayer_StatefulObjectFilterIdleMaxTicks:\t" << opticalLayerParameters.statefulObjectFilterIdleMaxTicks << endl;
	cout << "\tOpticalLayer_StatefulObjectFilterTrajectoryMinStates:\t" << opticalLayerParameters.statefulObjectFilterTrajectoryMinStates << endl;
	cout << "[DUO Stereo Camera]" << endl;
	cout << "\tGain:\t\t" << duoHardwareParameters.gain << endl;
	cout << "\tExposure:\t\t" << duoHardwareParameters.exposure << endl;
	cout << "\tLeds:\t\t" << duoHardwareParameters.leds << endl;
	cout << "[ZED Stereo Camera]" << endl;
	cout << "\tPerformance Mode:\t\t" << zedHardwareParameters.performanceMode << endl;
	cout << "\tSensing Mode:\t\t" << zedHardwareParameters.sensingMode << endl;
	cout << "\tDepth Frame Interleave:\t\t" << zedHardwareParameters.depthFrameInterleave << endl;
	cout << "[Interpolation Engine]" << endl;
	cout << "\tExtrema Neighbourhood Size:\t" << interpolationEngineParameters.extremaNeighbourhoodSize << endl;

	cout << "[Calibration Data]\n" << endl;
	cout << "CalibrationData_TGT_Hmin: " << calibrationData.targetHSVRange.Hmin << endl;
	cout << "CalibrationData_TGT_Smin: " << calibrationData.targetHSVRange.Smin << endl;
	cout << "CalibrationData_TGT_Vmin: " << calibrationData.targetHSVRange.Vmin << endl;
	cout << "CalibrationData_TGT_Hmax: " << calibrationData.targetHSVRange.Hmax << endl;
	cout << "CalibrationData_TGT_Smax: " << calibrationData.targetHSVRange.Smax << endl;
	cout << "CalibrationData_TGT_Vmax: " << calibrationData.targetHSVRange.Vmax << endl;
}

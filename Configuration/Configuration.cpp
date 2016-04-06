/*
 * Configuration.cpp
 *
 *  Created on: Feb 1, 2016
 *      Author: sled
 */

#include "Configuration.h"

#include <zed/Camera.hpp>
#include <zed/utils/GlobalDefine.hpp>

using namespace sl;

Configuration *Configuration::getInstance()
{
	static Configuration *instance = 0;

	if (!instance) {
		instance = new Configuration();
	}

	return instance;
}

Configuration::Configuration() {
}

Configuration::~Configuration() {
}

void Configuration::setOperationalMode(InputSensorDevice inputDevice, ProcessingMode processingMode)
{
	operationalMode.inputDevice = inputDevice;
	operationalMode.processingMode = processingMode;
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

	// Interpolation Engine
	fs << "InterpolationEngine_ExtremaNeighbourhoodSize" << interpolationEngineParameters.extremaNeighbourhoodSize;

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

	switch (zedHardwareParameters.performanceMode)
	{
	case HighPerformance:
		zedHardwareParameters.performanceModeIntValue = zed::MODE::PERFORMANCE;
		break;
	case QualityAcquisition:
		zedHardwareParameters.performanceModeIntValue = zed::MODE::QUALITY;
		break;

	default:
		zedHardwareParameters.performanceModeIntValue = zed::MODE::PERFORMANCE;
		break;

	}

	fs["StereoCameraZED_SensingMode"] >> iTmp;
		zedHardwareParameters.sensingMode = static_cast<ZEDSensingMode>(iTmp);

		switch (zedHardwareParameters.sensingMode)
		{
		case RawSensing:
			zedHardwareParameters.sensingModeIntValue = zed::SENSING_MODE::RAW;
			break;
		case FullSensing:
			zedHardwareParameters.sensingModeIntValue = zed::SENSING_MODE::FULL;
			break;

		default:
			zedHardwareParameters.sensingModeIntValue = zed::SENSING_MODE::RAW;
			break;

		}

	fs["StereoCameraZED_SensingMode"] >> duoHardwareParameters.exposure;

	// Interpolation Engine
	fs["InterpolationEngine_ExtremaNeighbourhoodSize"] >> interpolationEngineParameters.extremaNeighbourhoodSize;
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
	cout << "[Interpolation Engine]" << endl;
	cout << "\tExtrema Neighbourhood Size:\t" << interpolationEngineParameters.extremaNeighbourhoodSize << endl;
}

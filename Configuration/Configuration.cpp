/*
 * Configuration.cpp
 *
 *  Created on: Feb 1, 2016
 *      Author: sled
 */

#include "Configuration.h"

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
	cout << "\tLinear Low-Pass Filter X:\t\t" << opticalLayerParameters.linearLowPassFilterX << endl;
	cout << "\tLinear Low-Pass Filter Y:\t\t" << opticalLayerParameters.linearLowPassFilterY << endl;
	cout << "\tOpticalLayer_StatefulObjectFilterRelatedMaxDiffSphereX:\t" << opticalLayerParameters.statefulObjectFilterRelatedMaxDiffSphereX;
	cout << "\tOpticalLayer_StatefulObjectFilterRelatedMaxDiffSphereY:\t" << opticalLayerParameters.statefulObjectFilterRelatedMaxDiffSphereY;
	cout << "\tOpticalLayer_StatefulObjectFilterRelatedMaxDiffDirectionalX:\t" << opticalLayerParameters.statefulObjectFilterRelatedMaxDiffDirectionalX;
	cout << "\tOpticalLayer_StatefulObjectFilterRelatedMaxDiffDirectionalY:\t" << opticalLayerParameters.statefulObjectFilterRelatedMaxDiffDirectionalY;
	cout << "\tOpticalLayer_StatefulObjectFilterIdleMaxTicks:\t" << opticalLayerParameters.statefulObjectFilterIdleMaxTicks;
	cout << "\tOpticalLayer_StatefulObjectFilterTrajectoryMinStates:\t" <<
	cout << "[DUO Stereo Camera]" << endl;
	cout << "\tGain:\t\t" << duoHardwareParameters.gain << endl;
	cout << "\tExposure:\t\t" << duoHardwareParameters.exposure << endl;
	cout << "\tLeds:\t\t" << duoHardwareParameters.leds << endl;
}

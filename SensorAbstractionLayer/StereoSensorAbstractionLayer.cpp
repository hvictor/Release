/*
 * StereoSensorAbstractionLayer.cpp
 *
 *  Created on: Feb 1, 2016
 *      Author: sled
 */

#include "StereoSensorAbstractionLayer.h"

StereoSensorAbstractionLayer::StereoSensorAbstractionLayer() {
}

StereoSensorAbstractionLayer::~StereoSensorAbstractionLayer() {
}

StereoFrameSize StereoSensorAbstractionLayer::getStereoFrameSize()
{
	return frameSize;
}

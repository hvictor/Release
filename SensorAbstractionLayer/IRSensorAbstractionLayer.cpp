/*
 * IRSensorAbstractionLayer.cpp
 *
 *  Created on: Feb 1, 2016
 *      Author: sled
 */

#include "IRSensorAbstractionLayer.h"

IRSensorAbstractionLayer::IRSensorAbstractionLayer() {
}

IRSensorAbstractionLayer::~IRSensorAbstractionLayer() {
}

IrFrameSize IRSensorAbstractionLayer::getIrFrameSize()
{
	return frameSize;
}


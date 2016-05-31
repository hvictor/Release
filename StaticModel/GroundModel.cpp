/*
 * GroundModel.cpp
 *
 *  Created on: May 30, 2016
 *      Author: sled
 */

#include "GroundModel.h"
#include "../SensorAbstractionLayer/ZEDStereoSensorDriver.h"

GroundModel *getInstance()
{
	static GroundModel *inst = 0;

	if (inst == 0) {
		inst = new GroundModel();
	}

	return inst;
}

GroundModel::GroundModel()
{
}

GroundModel::~GroundModel()
{
}

PlaneLinearModel GroundModel::computeGroundPlaneLinearModel(Point nearL, Point nearR, Point farL, Point farR)
{
	PlaneLinearModel groundPlaneLinearModel;

	// Request the Stereo Sensor's Driver instance
	printf("Ground Model :: Compute Ground Plane Linear Model :: Point NEAR, LEFT:\n");
	ZEDStereoSensorDriver::getInstance()->repeatedDepthMeasure(nearL.x, nearL.y);

	printf("Ground Model :: Compute Ground Plane Linear Model :: Point NEAR, RIGHT:\n");
	ZEDStereoSensorDriver::getInstance()->repeatedDepthMeasure(nearR.x, nearR.y);

	printf("Ground Model :: Compute Ground Plane Linear Model :: Point FAR, LEFT:\n");
	ZEDStereoSensorDriver::getInstance()->repeatedDepthMeasure(farL.x, farL.y);

	printf("Ground Model :: Compute Ground Plane Linear Model :: Point FAR, RIGHT:\n");
	ZEDStereoSensorDriver::getInstance()->repeatedDepthMeasure(farR.x, farR.y);

	return groundPlaneLinearModel;
}


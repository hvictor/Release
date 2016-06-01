/*
 * GroundModel.cpp
 *
 *  Created on: May 30, 2016
 *      Author: sled
 */

#include "GroundModel.h"
#include "../SensorAbstractionLayer/ZEDStereoSensorDriver.h"

GroundModel *GroundModel::getInstance()
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
	ZEDStereoSensorDriver *sensorDriverHdl;

	// Request ZED Sensor Driver Handle
	sensorDriverHdl = ZEDStereoSensorDriver::getInstance();

	// Measure four points' depth values with the highest hardware precision available
	printf("Floor Model :: Compute Ground Plane Linear Model :: Point NEAR, LEFT:\n");
	float depth_nearL = sensorDriverHdl->repeatedDepthMeasure(nearL.x, nearL.y);

	printf("Floor Model :: Compute Ground Plane Linear Model :: Point NEAR, RIGHT:\n");
	float depth_nearR = sensorDriverHdl->repeatedDepthMeasure(nearR.x, nearR.y);

	printf("Floor Model :: Compute Ground Plane Linear Model :: Point FAR, LEFT:\n");
	float depth_farL = sensorDriverHdl->repeatedDepthMeasure(farL.x, farL.y);

	printf("Floor Model :: Compute Ground Plane Linear Model :: Point FAR, RIGHT:\n");
	float depth_farR = sensorDriverHdl->repeatedDepthMeasure(farR.x, farR.y);

	// Triangulate vaules in order to get the corresponding 3D coordinates vector
	vector<Vector3D> measure_points;

	Vector3D vector_nearL = StereoVision::convertStereoMeasureToVector(sensorDriverHdl->triangulate(depth_nearL, nearL.x, nearL.y));
	Vector3D vector_nearR = StereoVision::convertStereoMeasureToVector(sensorDriverHdl->triangulate(depth_nearR, nearR.x, nearR.y));
	Vector3D vector_farL = StereoVision::convertStereoMeasureToVector(sensorDriverHdl->triangulate(depth_farL, farL.x, farL.y));
	Vector3D vector_farR = StereoVision::convertStereoMeasureToVector(sensorDriverHdl->triangulate(depth_farR, farR.x, farR.y));

	measure_points.push_back(vector_nearL);
	measure_points.push_back(vector_nearR);
	measure_points.push_back(vector_farR);
	measure_points.push_back(vector_farL);

	// Re-project triangulated 3D measurements on the Reference Camera's image plane for visual validation

	vector<Point2f> projections = StereoVision::project3DCoordinatesOnImagePlane(measure_points);

	// Fill Floor Linear Model
	groundPlaneLinearModel.nearL = vector_nearL;
	groundPlaneLinearModel.nearR = vector_nearR;
	groundPlaneLinearModel.farL = vector_farL;
	groundPlaneLinearModel.farR = vector_farR;

	groundPlaneLinearModel.proj_nearL = projections[0];
	groundPlaneLinearModel.proj_nearR = projections[1];
	groundPlaneLinearModel.proj_farR = projections[2];
	groundPlaneLinearModel.proj_farL = projections[3];

	return groundPlaneLinearModel;
}

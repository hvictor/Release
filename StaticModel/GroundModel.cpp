/*
 * GroundModel.cpp
 *
 *  Created on: May 30, 2016
 *      Author: sled
 */

#include "GroundModel.h"
#include "../SensorAbstractionLayer/ZEDStereoSensorDriver.h"
#include <math.h>

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

// Compute plane linear equation coefficients
void GroundModel::computeLinearEquationCoefficients(PlaneLinearModel *planeModel, Vector3D P1, Vector3D P2, Vector3D P3)
{
	//	ax + by + cz + d = 0
	//	P1 = (x1, y1, z1)
	//  P2 = (x2, y2, z2)
	//	P3 = (x3, y3, z3)
	//
	//  a = (y2-y1)*(z3-z1) - (y3-y1)*(z2-z1)
	//  b = (x2-x1)*(z3-z1) - (x3-x1)*(z2-z1)
	//	c = (x2-x1)*(y3-y1) - (x3-x1)*(y2-y1)
	//	d = -(a*x4 + b*y4 + c*z4)

	double x1 = P1.x;
	double y1 = P1.y;
	double z1 = P1.z;

	double x2 = P2.x;
	double y2 = P2.y;
	double z2 = P2.z;

	double x3 = P3.x;
	double y3 = P3.y;
	double z3 = P3.z;

	double x4 = P1.x;
	double y4 = P1.y;
	double z4 = P1.z;

	planeModel->a = (y2-y1)*(z3-z1) - (y3-y1)*(z2-z1);
	planeModel->b = (x2-x1)*(z3-z1) - (x3-x1)*(z2-z1);
	planeModel->c = (x2-x1)*(y3-y1) - (x3-x1)*(y2-y1);
	planeModel->d = -(planeModel->a*x4 + planeModel->b*y4 + planeModel->c*z4);
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

	printf("Floor Model :: Points:\n");
	printf("\t[NEAR L] = [%.2f, %.2f, %.2f]\n", vector_nearL.x, vector_nearL.y, vector_nearL.z);
	printf("\t[NEAR R] = [%.2f, %.2f, %.2f]\n", vector_nearR.x, vector_nearR.y, vector_nearR.z);
	printf("\t[FAR  R] = [%.2f, %.2f, %.2f]\n", vector_farR.x, vector_farR.y, vector_farR.z);
	printf("\t[FAR  L] = [%.2f, %.2f, %.2f]\n", vector_farL.x, vector_farL.y, vector_farL.z);

	// Re-project triangulated 3D measurements on the Reference Camera's image plane for visual validation

	vector<Point2f> projections = StereoVision::project3DCoordinatesOnImagePlane(measure_points);

	// Fill Floor Linear Model
	groundPlaneLinearModel.nearL = vector_nearL;
	groundPlaneLinearModel.nearR = vector_nearR;
	groundPlaneLinearModel.farL = vector_farL;
	groundPlaneLinearModel.farR = vector_farR;

	groundPlaneLinearModel.farL = vector_farL;
	groundPlaneLinearModel.farR = vector_farR;


	printf("\tPROJ: [NEAR L] = [%.2f, %.2f]\n", projections[0].x, projections[0].y);
	printf("\tPROJ: [NEAR R] = [%.2f, %.2f]\n", projections[1].x, projections[1].y);
	printf("\tPROJ: [FAR  R] = [%.2f, %.2f]\n", projections[2].x, projections[2].y);
	printf("\tPROJ: [FAR  L] = [%.2f, %.2f]\n", projections[3].x, projections[3].y);

	groundPlaneLinearModel.proj_nearL = projections[0];
	groundPlaneLinearModel.proj_nearR = projections[1];
	groundPlaneLinearModel.proj_farR = projections[2];
	groundPlaneLinearModel.proj_farL = projections[3];

	computeLinearEquationCoefficients(&groundPlaneLinearModel, vector_nearL, vector_nearR, vector_farL);
	printf("Floor Model :: Linear equation: %.2f*x + %.2f*y + %.2f*z + %.2f = 0\n", groundPlaneLinearModel.a, groundPlaneLinearModel.b, groundPlaneLinearModel.c, groundPlaneLinearModel.d);

	_planeLinearModel = groundPlaneLinearModel;

	return groundPlaneLinearModel;
}

PlaneLinearModel GroundModel::getGroundPlaneLinearModel()
{
	return _planeLinearModel;
}

double GroundModel::computeDistanceFromGroundPlane(Vector3D v)
{
	// dist = abs(a*vx + b*vy + c*vz + d) / sqrt(a^2 + b^2 + c^2)

	double a = _planeLinearModel.a;
	double b = _planeLinearModel.b;
	double c = _planeLinearModel.c;
	double d = _planeLinearModel.d;

	return (fabsl(a * v.x + b * v.y + c * v.z + d) / sqrt(a*a + b*b + c*c));
}

void GroundModel::setGroundPlaneLinearModelFactorX(double a)
{
	_planeLinearModel.a = a;
}

void GroundModel::setGroundPlaneLinearModelFactorY(double b)
{
	_planeLinearModel.b = b;
}

void GroundModel::setGroundPlaneLinearModelFactorZ(double c)
{
	_planeLinearModel.c = c;
}

void GroundModel::setGroundPlaneLinearModelFactorD(double d)
{
	_planeLinearModel.d = d;
}

Vector3D GroundModel::computePlaneNormalVector(PlaneLinearModel *planeModel)
{
	Vector3D normal;

	normal.x = planeModel->a;
	normal.y = planeModel->b;
	normal.z = planeModel->c;

	return normal;
}

PlaneReferenceSystemAxis GroundModel::computePlaneReferenceSystemAxis(PlaneLinearModel *planeModel)
{
	PlaneReferenceSystemAxis axis;

	Vector3D normal = computePlaneNormalVector(planeModel);

	// Using Near, Left as visual origin
	Vector3D visualAxisEndPointX = planeModel->nearR;

	Vector3D visualAxisEndPointZ = planeModel->farL;
	visualAxisEndPointZ.z *= -1.0;

	Vector3D visualOrigin = planeModel->nearL;

	normal.x += visualOrigin.x;
	normal.y += visualOrigin.y;
	normal.z += visualOrigin.z;

	axis.yAxisFrom = visualOrigin;
	axis.yAxisTo = normal;

	axis.xAxisFrom = visualOrigin;
	axis.xAxisTo = visualAxisEndPointX;

	axis.zAxisFrom = visualOrigin;
	axis.zAxisTo = visualAxisEndPointZ;

	return axis;
}

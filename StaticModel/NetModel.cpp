/*
 * NetModel.cpp
 *
 *  Created on: May 30, 2016
 *      Author: sled
 */

#include "NetModel.h"
#include "../SensorAbstractionLayer/ZEDStereoSensorDriver.h"
#include <math.h>

NetModel *NetModel::getInstance()
{
	static NetModel *inst = 0;

	if (inst == 0) {
		inst = new NetModel();
	}

	return inst;
}

NetModel::NetModel()
{
}

NetModel::~NetModel()
{
}

void NetModel::measureBasePointNear(int x, int y)
{
	ZEDStereoSensorDriver *sensorDriverHdl;

	// Request ZED Sensor Driver Handle
	sensorDriverHdl = ZEDStereoSensorDriver::getInstance();

	// Measure points' depth value with the highest hardware precision available
	printf("Net Model :: Measuring Near Base Point\n");
	float depth_baseNear = sensorDriverHdl->repeatedDepthMeasure(x, y);

	printf("Net Model :: Triangulating Near Base Points\n");
	_netCoordinates3D.baseNear = StereoVision::convertStereoMeasureToVector(sensorDriverHdl->triangulate(depth_baseNear, x, y));
}

void NetModel::measureBasePointFar(int x, int y)
{
	ZEDStereoSensorDriver *sensorDriverHdl;

	// Request ZED Sensor Driver Handle
	sensorDriverHdl = ZEDStereoSensorDriver::getInstance();

	// Measure points' depth value with the highest hardware precision available
	printf("Net Model :: Measuring Far Base Point\n");
	float depth_baseFar = sensorDriverHdl->repeatedDepthMeasure(x, y);

	printf("Net Model :: Triangulating Far Base Points\n");
	_netCoordinates3D.baseFar = StereoVision::convertStereoMeasureToVector(sensorDriverHdl->triangulate(depth_baseFar, x, y));
}

void NetModel::setHeight(double height_mm)
{
	_netCoordinates3D.topNear.x = _netCoordinates3D.baseNear.x;
	_netCoordinates3D.topNear.y = _netCoordinates3D.baseNear.y - height_mm;
	_netCoordinates3D.topNear.z = _netCoordinates3D.baseNear.z;

	_netCoordinates3D.topFar.x = _netCoordinates3D.baseFar.x;
	_netCoordinates3D.topFar.y = _netCoordinates3D.baseFar.y - height_mm;
	_netCoordinates3D.topFar.z = _netCoordinates3D.baseFar.z;
}

void NetModel::computeImagePlaneProjections()
{
	vector<Vector3D> measure_points;

	measure_points.push_back(_netCoordinates3D.baseNear);
	measure_points.push_back(_netCoordinates3D.baseFar);
	measure_points.push_back(_netCoordinates3D.topNear);
	measure_points.push_back(_netCoordinates3D.topFar);

	// Re-project triangulated 3D measurements on the Reference Camera's image plane for visual validation
	vector<Point2f> projections = StereoVision::project3DCoordinatesOnImagePlane(measure_points);

	_netVisualProjection.baseNear = projections[0];
	_netVisualProjection.baseFar = projections[1];
	_netVisualProjection.topNear = projections[2];
	_netVisualProjection.topFar = projections[3];
}

NetVisualProjection NetModel::getNetVisualProjection()
{
	return _netVisualProjection;
}

NetCoordinates3D NetModel::getNetCoordinates3D()
{
	return _netCoordinates3D;
}

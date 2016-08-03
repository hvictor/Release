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
	_ready = false;
}

NetModel::~NetModel()
{
}

void NetModel::setReady(bool ready)
{
	_ready = ready;
}

bool NetModel::isReady()
{
	return _ready;
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

void NetModel::computeNetVisualPoints()
{
	/*
	double deltay = (_netCoordinates3D.baseNear.y - _netCoordinates3D.topNear.y) * 0.1;
	double deltaz = (_netCoordinates3D.baseFar.z - _netCoordinates3D.baseNear.z) * 0.1;
	double deltax = (_netCoordinates3D.baseFar.x - _netCoordinates3D.baseNear.x) * 0.1;
	*/

	_netVisualPoints.clear();

	vector<Vector3D> vs;

	/*
	for (int z = 0; z < 10; z++) {
		for (int y = 0; y <= 10; y++) {
			Vector3D v;
			v.x = _netCoordinates3D.baseNear.x + ((double)z)*deltax;
			v.y = _netCoordinates3D.topNear.y + ((double)y)*deltay;
			v.z = _netCoordinates3D.baseNear.z + ((double)z)*deltaz;
			vs.push_back(v);
		}
	}
	*/

	double deltay = (_netCoordinates3D.baseNear.y - _netCoordinates3D.topNear.y) * 0.25;
	double deltaz = (_netCoordinates3D.baseFar.z - _netCoordinates3D.baseNear.z) * 0.25;
	double deltax = (_netCoordinates3D.baseFar.x - _netCoordinates3D.baseNear.x) * 0.25;

	// Base points and top points
	for (double z = 1.0; z <= 5.0; z += 1.0) {
		Vector3D v;
		v.x = _netCoordinates3D.baseNear.x + z*deltax;
		v.y = _netCoordinates3D.baseNear.y;
		v.z = _netCoordinates3D.baseNear.z + z*deltaz;
		vs.push_back(v);
	}
	for (double z = 1.0; z <= 5.0; z += 1.0) {
		Vector3D v;
		v.x = _netCoordinates3D.baseNear.x + z*deltax;
		v.y = _netCoordinates3D.topNear.y;
		v.z = _netCoordinates3D.baseNear.z + z*deltaz;
		vs.push_back(v);
	}

	// Left and right points
	for (double y = 1.0; y <= 5.0; y += 1.0) {
		Vector3D v;
		v.x = _netCoordinates3D.baseNear.x;
		v.y = _netCoordinates3D.topNear.y + y*deltay;
		v.z = _netCoordinates3D.baseNear.z;
		vs.push_back(v);
	}
	for (double y = 1.0; y <= 5.0; y += 1.0) {
		Vector3D v;
		v.x = _netCoordinates3D.baseFar.x;
		v.y = _netCoordinates3D.topFar.y + y*deltay;
		v.z = _netCoordinates3D.baseFar.z;
		vs.push_back(v);
	}

	_netVisualPoints = StereoVision::project3DCoordinatesOnImagePlane(vs);
}

vector<Point2f> NetModel::getNetVisualPoints()
{
	return _netVisualPoints;
}

NetVisualProjection NetModel::getNetVisualProjection()
{
	return _netVisualProjection;
}

void NetModel::setNetVisualProjection(NetVisualProjection netVisualProjection)
{
	_netVisualProjection = netVisualProjection;
}

NetCoordinates3D NetModel::getNetCoordinates3D()
{
	return _netCoordinates3D;
}

void NetModel::setNetCoordinates3D(NetCoordinates3D netCoordinates3D)
{
	_netCoordinates3D = netCoordinates3D;
}

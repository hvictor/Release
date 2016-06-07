/*
 * GroundModel.h
 *
 *  Created on: May 30, 2016
 *      Author: sled
 */

#ifndef GROUNDMODEL_H_
#define GROUNDMODEL_H_

#include "../Configuration/Configuration.h"
#include "../StereoVision/StereoVision.h"
#include "../Common/opencv_headers.h"
#include "../Common/data_types.h"

using namespace std;
using namespace cv;

typedef struct
{
	Point2f baseNear;
	Point2f baseFar;
	Point2f topNear;
	Point2f topFar;
} NetVisualProjection;

typedef struct
{
	Vector3D baseNear;
	Vector3D baseFar;
	Vector3D topNear;
	Vector3D topFar;
} NetCoordinates3D;

class NetModel {
public:
	static NetModel *getInstance();
	virtual ~NetModel();
	void measureBasePointNear(int x, int y);
	void measureBasePointFar(int x, int y);
	void setHeight(double height_mm);
	void computeImagePlaneProjections();
	NetVisualProjection getNetVisualProjection();
	NetCoordinates3D getNetCoordinates3D();

private:
	NetModel();

	NetCoordinates3D	_netCoordinates3D;
	NetVisualProjection _netVisualProjection
};

#endif /* GROUNDMODEL_H_ */

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
	// Plane equation: a*x + b*y + c*z + d = 0
	double a;
	double b;
	double c;
	double d;

	// Points of appoggio
	Vector3D nearL, nearR;
	Vector3D farL, farR;

	// Projections on Image Plane
	Point2f proj_nearL, proj_nearR;
	Point2f proj_farL, proj_farR;

} PlaneLinearModel;

class GroundModel {
public:
	static GroundModel *getInstance();
	virtual ~GroundModel();
	PlaneLinearModel computeGroundPlaneLinearModel(Point nearL, Point nearR, Point farL, Point farR);
	PlaneLinearModel getGroundPlaneLinearModel();
	void computeLinearEquationCoefficients(PlaneLinearModel *planeModel, Vector3D P1, Vector3D P2, Vector3D P3);
	double computeDistanceFromGroundPlane(Vector3D v);

private:
	GroundModel();
	PlaneLinearModel _planeLinearModel;

};

#endif /* GROUNDMODEL_H_ */

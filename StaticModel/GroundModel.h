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
	// Plane equation

	// Points of appoggio
	Vector3D nearL, nearR;
	Vector3D farL, farR;

} PlaneLinearModel;

class GroundModel {
public:
	static GroundModel *getInstance();
	virtual ~GroundModel();
	PlaneLinearModel computeGroundPlaneLinearModel(Point nearL, Point nearR, Point farL, Point farR);

private:
	GroundModel();

};

#endif /* GROUNDMODEL_H_ */

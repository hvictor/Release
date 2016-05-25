/*
 * StereoVision.h
 *
 *  Created on: Feb 3, 2016
 *      Author: sled
 */

#ifndef STEREOVISION_H_
#define STEREOVISION_H_

#include "../Common/opencv_headers.h"
#include "../Common/data_types.h"
#include "IntrinsicParameters.h"
#include "ExtrinsicParameters.h"

typedef struct
{
	float x;
	float y;
	float z;
} StereoMeasureXYZ;

class StereoVision {
public:
	StereoVision();
	vector<Point2f> project3DCoordinatesOnImagePlane(CameraIntrinsics intrinsics, ExtrinsicParameters extrinsics, vector<Vector3D> coords_3d);
	virtual ~StereoVision();
};

#endif /* STEREOVISION_H_ */

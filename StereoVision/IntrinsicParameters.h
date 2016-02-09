/*
 * IntrinsicParameters.h
 *
 *  Created on: Feb 3, 2016
 *      Author: sled
 */

#ifndef INTRINSICPARAMETERS_H_
#define INTRINSICPARAMETERS_H_

#include "../Common/opencv_headers.h"

typedef struct
{
	double lensRadialDistortionCoeffs[6]; // k1-k6
	double tangentialDistortionCoeffs[2]; // p1-p2
	double focalLenCoeffX;
	double focalLenCoeffY;
	double centerOfProjectionX;
	double centerOfProjectionY;
	Mat cameraMatrix;
} CameraIntrinsics;

class IntrinsicParameters {
public:
	IntrinsicParameters(double *intrinsicsL, double *intrinsicsR);
	virtual ~IntrinsicParameters();
	CameraIntrinsics getIntrinsicsLeft();
	CameraIntrinsics getIntrinsicsRight();
private:
	CameraIntrinsics cameraIntrL;
	CameraIntrinsics cameraIntrR;
};

#endif /* INTRINSICPARAMETERS_H_ */

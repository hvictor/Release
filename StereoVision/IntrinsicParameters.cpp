/*
 * IntrinsicParameters.cpp
 *
 *  Created on: Feb 3, 2016
 *      Author: sled
 */

#include "IntrinsicParameters.h"

IntrinsicParameters::IntrinsicParameters(double *intrinsicsL, double *intrinsicsR)
{
	cameraIntrL.cameraMatrix = Mat(3, 3, CV_32FC1);
	cameraIntrR.cameraMatrix = Mat(3, 3, CV_32FC1);

	for (int i = 0; i < 6; i++) {
		cameraIntrL.lensRadialDistortionCoeffs[i] = intrinsicsL[i];
		cameraIntrR.lensRadialDistortionCoeffs[i] = intrinsicsR[i];
	}

	cameraIntrL.tangentialDistortionCoeffs[0] = intrinsicsL[6];
	cameraIntrL.lensRadialDistortionCoeffs[1] = intrinsicsL[7];
	cameraIntrL.focalLenCoeffX = intrinsicsL[8];
	cameraIntrL.focalLenCoeffY = intrinsicsL[9];
	cameraIntrL.centerOfProjectionX = intrinsicsL[10];
	cameraIntrL.centerOfProjectionY = intrinsicsL[11];

	cameraIntrL.cameraMatrix.at<float>(0,0) = cameraIntrL.focalLenCoeffX; 		// fx
	cameraIntrL.cameraMatrix.at<float>(0,1) = 0.0;								// 0
	cameraIntrL.cameraMatrix.at<float>(0,2) = cameraIntrL.centerOfProjectionX;	// Cx
	cameraIntrL.cameraMatrix.at<float>(1,0) = 0.0;								// 0
	cameraIntrL.cameraMatrix.at<float>(1,1) = cameraIntrL.focalLenCoeffY;		// fy
	cameraIntrL.cameraMatrix.at<float>(1,2) = cameraIntrL.centerOfProjectionY;	// Cy
	cameraIntrL.cameraMatrix.at<float>(2,0) = 0.0;             					// 0
	cameraIntrL.cameraMatrix.at<float>(2,1) = 0.0;								// 0
	cameraIntrL.cameraMatrix.at<float>(2,2) = 1.0;								// 1

	cameraIntrR.cameraMatrix.at<float>(0,0) = cameraIntrR.focalLenCoeffX; 		// fx
	cameraIntrR.cameraMatrix.at<float>(0,1) = 0.0;								// 0
	cameraIntrR.cameraMatrix.at<float>(0,2) = cameraIntrR.centerOfProjectionX;	// Cx
	cameraIntrR.cameraMatrix.at<float>(1,0) = 0.0;								// 0
	cameraIntrR.cameraMatrix.at<float>(1,1) = cameraIntrR.focalLenCoeffY;		// fy
	cameraIntrR.cameraMatrix.at<float>(1,2) = cameraIntrR.centerOfProjectionY;	// Cy
	cameraIntrR.cameraMatrix.at<float>(2,0) = 0.0;             					// 0
	cameraIntrR.cameraMatrix.at<float>(2,1) = 0.0;								// 0
	cameraIntrR.cameraMatrix.at<float>(2,2) = 1.0;								// 1
}

IntrinsicParameters::~IntrinsicParameters()
{
}

CameraIntrinsics IntrinsicParameters::getIntrinsicsLeft()
{
	return cameraIntrL;
}

CameraIntrinsics IntrinsicParameters::getIntrinsicsRight()
{
	return cameraIntrR;
}

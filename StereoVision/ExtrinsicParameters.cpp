/*
 * ExtrinsicParameters.cpp
 *
 *  Created on: Feb 3, 2016
 *      Author: sled
 */

#include "ExtrinsicParameters.h"

ExtrinsicParameters::ExtrinsicParameters(double *rotation, double *translation)
{
	rotationMatrix = Mat(3, 3, CV_32F);
	rotationVector = Mat(1, 3, CV_32FC1);
	translationVector = Mat(1, 3, CV_32FC1);

	rotationMatrix.at<float>(0,0) = rotation[0];
	rotationMatrix.at<float>(0,1) = rotation[1];
	rotationMatrix.at<float>(0,2) = rotation[2];
	rotationMatrix.at<float>(1,0) = rotation[3];
	rotationMatrix.at<float>(1,1) = rotation[4];
	rotationMatrix.at<float>(1,2) = rotation[5];
	rotationMatrix.at<float>(2,0) = rotation[6];
	rotationMatrix.at<float>(2,1) = rotation[7];
	rotationMatrix.at<float>(2,2) = rotation[8];

	/* Legacy: parameters for DUO
	rotationMatrix.at<float>(0,0) = 0.999988;
	rotationMatrix.at<float>(0,1) = 0.001198;
	rotationMatrix.at<float>(0,2) = 0.004648;
	rotationMatrix.at<float>(1,0) = -0.001155;
	rotationMatrix.at<float>(1,1) = 0.999955;
	rotationMatrix.at<float>(1,2) = -0.009381;
	rotationMatrix.at<float>(2,0) = -0.004659;
	rotationMatrix.at<float>(2,1) = 0.009375;
	rotationMatrix.at<float>(2,2) = 0.999945;
	*/

	translationVector.at<float>(0, 0) = translation[0];
	translationVector.at<float>(0, 1) = translation[1];
	translationVector.at<float>(0, 2) = translation[2];

	/* Legacy: parameters for DUO
	tvecR.at<float>(0, 0) = -30.262322;
	tvecR.at<float>(0, 1) = 0.083806;
	tvecR.at<float>(0, 2) = -0.575650;
	*/

	Rodrigues(rotationMatrix, rotationVector);
	transpose(rotationVector, rotationVector);
}

ExtrinsicParameters::~ExtrinsicParameters()
{
}

Mat ExtrinsicParameters::getRotationMatrix()
{
	return rotationMatrix;
}

Mat ExtrinsicParameters::getRotationVector()
{
	return rotationVector;
}

Mat ExtrinsicParameters::getTranslationVector()
{
	return translationVector;
}


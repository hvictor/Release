/*
 * ExtrinsicParameters.h
 *
 *  Created on: Feb 3, 2016
 *      Author: sled
 */

#ifndef EXTRINSICPARAMETERS_H_
#define EXTRINSICPARAMETERS_H_

#include "../Common/opencv_headers.h"

//
// Extrinsic parameters: transforms from reference camera to the other camera spaces.
// In this project: reference camera is Left
// Note: the Left translation vector is made of zeroes
//

class ExtrinsicParameters {
public:
	ExtrinsicParameters(double *rotation, double *translation);
	virtual ~ExtrinsicParameters();
	Mat getRotationMatrix();
	Mat getRotationVector();
	Mat getTranslationVector();

private:
	Mat rotationMatrix;
	Mat rotationVector;
	Mat translationVector;
};

#endif /* EXTRINSICPARAMETERS_H_ */

/*
 * TennisFieldStaticModel.h
 *
 *  Created on: Jan 29, 2016
 *      Author: sled
 */

#ifndef TENNISFIELDSTATICMODEL_H_
#define TENNISFIELDSTATICMODEL_H_

#include "../Common/opencv_headers.h"
#include "TennisField3D.h"

class TennisFieldStaticModel {
public:
	TennisFieldStaticModel();
	virtual ~TennisFieldStaticModel();
	static void computeTennisFieldVisualPoints(Mat objectPoints, Mat& visualPoints3D, double h_displacement);
	static void computeTennisNetVisualPoints(Mat objectPoints, Mat& visualPoints3D, double h_displacement);
	static int checkTennisField_2D(TennisField_3D *field, double x, double y);
};

#endif /* TENNISFIELDSTATICMODEL_H_ */

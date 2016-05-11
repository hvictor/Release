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
#include "../Calibration/TennisFieldDelimiter.h"

class TennisFieldStaticModel {
public:
	static TennisFieldStaticModel *getInstance();
	void computeTennisFieldVisualPoints(Mat objectPoints, Mat& visualPoints3D, double h_displacement);
	void computeTennisNetVisualPoints(Mat objectPoints, Mat& visualPoints3D, double h_displacement);
	void setTennisFieldDelimiter(TennisFieldDelimiter *fieldDelimiter);
	int getScore();
	int getLineHits();
	int updateScoreTracking_2D(double x, double y);
	TennisFieldDelimiter *getTennisFieldDelimiter();
private:
	TennisFieldStaticModel();
	virtual ~TennisFieldStaticModel();
	int checkTennisField_2D(TennisField_3D *field, double x, double y);
	TennisFieldDelimiter *fieldDelimiter;
	vector<Point2f> score_positions;
	int _score;
	int _line;
};

#endif /* TENNISFIELDSTATICMODEL_H_ */

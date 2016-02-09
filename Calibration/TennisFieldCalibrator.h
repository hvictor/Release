/*
 * TennisFieldCalibrator.h
 *
 *  Created on: Jan 29, 2016
 *      Author: sled
 */

#ifndef TENNISFIELDCALIBRATOR_H_
#define TENNISFIELDCALIBRATOR_H_

#include "GPULineDetector.h"
#include "CoarseSegmentProcessor.h"
#include "IntersectionPointsDetector.h"
#include "CalibrationWindow.h"
#include "TennisFieldDelimiter.h"

class TennisFieldCalibrator {
public:
	TennisFieldCalibrator();
	virtual ~TennisFieldCalibrator();
	void regulateVisualCalibrationWindow(Mat calibrationFrame);
	void setCalibrationWindow(Point2f center, double h, double basis_min, double basis_maj, double h_displacement);
	TennisFieldDelimiter *computeTennisFieldDelimiter(Mat calibrationFrame, vector<Point> intersPts, CalibrationWindow *calibWnd);
	Point findClosestPoint(vector<Point> pts, Point p);
	TennisFieldDelimiter *calibrate(Mat calibrationFrame);
	GPULineDetector *getCUDALinesDetector();

private:
	GPULineDetector *gpuLineDetector;
	CoarseSegmentProcessor *coarseSegmentProcessor;
	IntersectionPointsDetector *intersDetector;
	CalibrationWindow *calibrationWindow;
	void renderCalibrationWindow(Mat calibrationFrame, CalibrationWindow *calibrationWindow);
	void renderCalibrationWindow(Mat calibrationFrame, double h, double basis_min, double basis_maj, double h_displacement);
};

#endif /* TENNISFIELDCALIBRATOR_H_ */

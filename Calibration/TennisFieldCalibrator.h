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
#include "PerimetralConesDetector.h"
#include <stdint.h>

class TennisFieldCalibrator {
public:
	TennisFieldCalibrator();
	virtual ~TennisFieldCalibrator();
	void regulateVisualCalibrationWindow(Mat calibrationFrame);
	void setCalibrationWindow(Point2f center, double h, double basis_min, double basis_maj, double h_displacement);
	TennisFieldDelimiter *computeTennisFieldDelimiter(Mat calibrationFrame, vector<Point> intersPts, CalibrationWindow *calibWnd);
	Point findClosestPoint(vector<Point> pts, Point p);
	TennisFieldDelimiter *calibrate(Mat calibrationFrame);
	TennisFieldDelimiter *calibrate_8UC4(uint8_t *u8data, int width, int height, bool *status);
	GPULineDetector *getCUDALinesDetector();
	void setPerimetralCones(PerimetralConeSet4 cone_set);
	PerimetralConeSet4 getPerimetralCones();
	TennisFieldDelimiter *computeConeDelimitedStaticModel(vector<Point> intersPts);

private:
	GPULineDetector *gpuLineDetector;
	CoarseSegmentProcessor *coarseSegmentProcessor;
	IntersectionPointsDetector *intersDetector;
	CalibrationWindow *calibrationWindow;
	void renderCalibrationWindow(Mat calibrationFrame, CalibrationWindow *calibrationWindow);
	void renderCalibrationWindow(Mat calibrationFrame, double h, double basis_min, double basis_maj, double h_displacement);
	PerimetralConeSet4 cones;
};

#endif /* TENNISFIELDCALIBRATOR_H_ */

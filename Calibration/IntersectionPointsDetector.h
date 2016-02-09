/*
 * IntersectionPointsDetector.h
 *
 *  Created on: Jan 30, 2016
 *      Author: sled
 */

#ifndef INTERSECTIONPOINTSDETECTOR_H_
#define INTERSECTIONPOINTSDETECTOR_H_

#include "GPULineDetector.h"
#include "CalibrationWindow.h"

class IntersectionPointsDetector {
public:
	IntersectionPointsDetector(CalibrationWindow *calibrationWindow);
	virtual ~IntersectionPointsDetector();
	vector<Point> computeIntersectionPoints(vector<Line> lines);
	bool pointInRange(Point2f q, Point2f p0, Point2f p1);
	bool pointLiesOnSegment(Point2f p, Line l);

private:
	bool findIntersectionPoint(Line l1, Line l2, Point2f *res);
	CalibrationWindow *calibrationWindow;
};

#endif /* INTERSECTIONPOINTSDETECTOR_H_ */

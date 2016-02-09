/*
 * CoarseSegmentProcessor.h
 *
 *  Created on: Jan 30, 2016
 *      Author: sled
 */

#ifndef COARSESEGMENTPROCESSOR_H_
#define COARSESEGMENTPROCESSOR_H_

#include "GPULineDetector.h"

class CoarseSegmentProcessor {
public:
	CoarseSegmentProcessor();
	virtual ~CoarseSegmentProcessor();
	vector<Line> process(vector<Line> coarseLines);

private:
	bool nearColinear(Line segment1, Line segment2, double angle_threshold, double endpoints_dist_thresh);
	double computeEndpointsDistance(Line segment1, Line segment2);
	Line average(Line line1, Line line2);
	Point averagePoint(Point p1, Point p2);
};

#endif /* COARSESEGMENTPROCESSOR_H_ */

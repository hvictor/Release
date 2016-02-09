/*
 * CoarseSegmentProcessor.cpp
 *
 *  Created on: Jan 30, 2016
 *      Author: sled
 */

#include "CoarseSegmentProcessor.h"
#include <math.h>

CoarseSegmentProcessor::CoarseSegmentProcessor() {
}

CoarseSegmentProcessor::~CoarseSegmentProcessor() {
}

double CoarseSegmentProcessor::computeEndpointsDistance(Line segment1, Line segment2)
{
	double deltax = segment2.p0.x - segment1.p1.x;
	double deltay = segment2.p0.y - segment1.p1.y;
	return sqrt(deltax*deltax + deltay*deltay);
}

bool CoarseSegmentProcessor::nearColinear(Line segment1, Line segment2, double angle_threshold, double endpoints_dist_thresh)
{
	// Near parallelism test
	double v1_x = segment1.p1.x - segment1.p0.x;
	double v1_y = segment1.p1.y - segment1.p0.y;

	double v2_x = segment2.p1.x - segment2.p0.x;
	double v2_y = segment2.p1.y - segment2.p0.y;

	double inner_prod = v1_x*v2_x + v1_y*v2_y;
	double norm1 = sqrt(v1_x*v1_x + v1_y*v1_y);
	double norm2 = sqrt(v2_x*v2_x + v2_y*v2_y);

	if (norm1*norm2 == 0) return false;

	double a = acos(inner_prod / (norm1*norm2));

	// If the angle is circa 0 or circa 180, the vectors are near parallel and need to continue
	if (fabsl(a) > angle_threshold && fabsl(180.0 - a) > angle_threshold) {
		return false;
	}

	// Check endpoints distance
	if (computeEndpointsDistance(segment1, segment2) <= endpoints_dist_thresh ||
		computeEndpointsDistance(segment2, segment1) <= endpoints_dist_thresh) {
		return true;
	}

	// Check that the angle between the two vectors, respectivey, and the vector between the endpoints, is unrder the threshold
	double evx = segment2.p1.x - segment1.p0.x;
	double evy = segment2.p1.y - segment1.p0.y;
	double norm_evx = sqrt(evx*evx + evy*evy);
	inner_prod = v1_x*evx + v1_y*evy;

	if (norm_evx*norm1 == 0) return false;

	a = acos(inner_prod / (norm_evx*norm1));

	if (fabsl(a) > angle_threshold && fabsl(180.0 - a) > angle_threshold) {
		return false;
	}

	return true;
}

Point CoarseSegmentProcessor::averagePoint(Point p1, Point p2)
{
	Point result;

	result.x = std::min(p2.x, p1.x) + (std::max(p2.x, p1.x) - std::min(p2.x, p1.x)) / 2;
	result.y = std::min(p2.y, p1.y) + (std::max(p2.y, p1.y) - std::min(p2.y, p1.y)) / 2;

	return result;
}

Line CoarseSegmentProcessor::average(Line line1, Line line2)
{
	Line result;

	result.p0 = averagePoint(line1.p0, line2.p0);
	result.p1 = averagePoint(line1.p1, line2.p1);

	return result;
}

vector<Line> CoarseSegmentProcessor::process(vector<Line> coarseLines)
{
	vector<Line> processed = coarseLines;

	bool running = true;

	while (running) {
		vector<Line> tmp;
		running = false;

		for (int i = 0; i < processed.size() - 1; i++) {
			for (int j = i + 1; j < processed.size(); j++) {
				if (nearColinear(processed[i], processed[j], 3.0, 20.0)) {
					tmp.push_back(average(processed[i], processed[j]));
					running = true;
				}
				else {
					tmp.push_back(processed[i]);
					tmp.push_back(processed[j]);
				}
			}
		}

		if (running) {
			processed = tmp;
		}
	}


	return processed;
}

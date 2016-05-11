/*
 * IntersectionPointsDetector.cpp
 *
 *  Created on: Jan 30, 2016
 *      Author: sled
 */

#include "IntersectionPointsDetector.h"
#include <stdio.h>

IntersectionPointsDetector::IntersectionPointsDetector(CalibrationWindow *calibrationWindow):
	_use_cones(false)
{
	this->calibrationWindow = calibrationWindow;
}

IntersectionPointsDetector::IntersectionPointsDetector(PerimetralConeSet4 cone_set):
	_use_cones(true),
	calibrationWindow(0)
{
	_cone_set = cone_set;
}

IntersectionPointsDetector::~IntersectionPointsDetector() {
}

bool IntersectionPointsDetector::pointInRange(Point2f q, Point2f p0, Point2f p1)
{
	double xmin = p0.x;
	double xmax = p1.x;
	double ymin = p0.y;
	double ymax = p1.y;

	if (xmin > xmax) {
		xmin = p1.x;
		xmax = p0.x;
	}

	if (ymin > ymax) {
		ymin = p1.y;
		ymax = p0.y;
	}

	return ((xmin <= q.x && q.x <= xmax) && (ymin <= q.y && q.y <= ymax));
}

bool IntersectionPointsDetector::pointLiesOnSegment(Point2f p, Line l)
{
	// y = mx + q
	// q = (y - mx)
	// m = (y1 - y0) / (x1 - x0)

	double m = (l.p1.y - l.p0.y) / (l.p1.x - l.p0.x);
	double q = l.p0.y - m * l.p0.x;
	double r = m * p.x + q;
	double tol = 0.01; // tolerance

	if (fabsl(r - p.y) < tol && pointInRange(p, l.p0, l.p1)) {
		return true;
	}

	return false;
}

bool IntersectionPointsDetector::findIntersectionPoint(Line l1, Line l2, Point2f *res)
{
    Point2f p = l1.p0;
    Point2f dp;
    Point2f q = l2.p0;
    Point2f dq;
    Point2f qmp; // q-p
    double dpdq_cross;     // 2 cross products
    double qpdq_cross;     // dp with dq,  q-p with dq
    double a;

    dp = l1.p1 - l1.p0;
    dq = l2.p1 - l2.p0;

    qmp = q - p;

    dpdq_cross = dp.x * dq.y - dp.y * dq.x;

    if (dpdq_cross < 0.0){
        return false;
    }

    qpdq_cross = qmp.x * dq.y - qmp.y * dq.x;
    a = (qpdq_cross*1.0f/dpdq_cross);

    res->x = p.x+a*dp.x;
    res->y = p.y+a*dp.y;
    return true;
}

vector<Point> IntersectionPointsDetector::computeIntersectionPoints(vector<Line> lines)
{
	vector<Point> results;

	printf("computeIntersectionPoints :: Creating calibration window using cones\n");

	CalibrationWindow *wnd = new CalibrationWindow();
	wnd->bottomLeft = _cone_set.vertex_bottomLeft;
	wnd->bottomRight = _cone_set.vertex_bottomRight;
	wnd->topLeft = _cone_set.vertex_topLeft;
	wnd->topRight = _cone_set.vertex_topRight;

	printf("computeIntersectionPoints :: Calibration window created, inspecting %d lines\n", lines.size());

	for (int i = 0; i < lines.size()-1; i++) {
		for (int j = 0; j < lines.size(); j++) {
			Point2f r;

			printf("computeIntersectionPoints :: Finding intersection point between lines %d and %d\n", i, j);
			if (findIntersectionPoint(lines[i], lines[j], &r) && pointLiesOnSegment(r, lines[i]) && pointLiesOnSegment(r, lines[j])) {
				if (wnd->containsPoint(r)) {
					results.push_back(r);
				}
			}
		}
	}

	delete wnd;

	printf("computeIntersectionPoints :: Done, RETURNING\n");

	return results;
}


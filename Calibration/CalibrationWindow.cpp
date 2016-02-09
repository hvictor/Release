/*
 * CalibrationWindow.cpp
 *
 *  Created on: Jan 31, 2016
 *      Author: sled
 */

#include "CalibrationWindow.h"
#include "../Common/point_in_polygon.h"

CalibrationWindow::CalibrationWindow() {
}

CalibrationWindow::~CalibrationWindow() {
}

bool CalibrationWindow::containsPoint(Point2f p)
{
	vector<Point2f> vertices;
	vertices.push_back(bottomLeft);
	vertices.push_back(bottomRight);
	vertices.push_back(topRight);
	vertices.push_back(topLeft);
	int c = (int)pointPolygonTest(vertices, p, false);

	return (c >= 0);
}

vector<Point> CalibrationWindow::filterPoints(vector<Point> points)
{
	vector<Point> filtered;

	for (int i = 0; i < points.size(); i++) {
		if (containsPoint(points[i])) {
			filtered.push_back(points[i]);
		}
	}

	return filtered;
}



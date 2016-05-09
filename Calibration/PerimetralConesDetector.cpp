/*
 * PerimetralConesDetector.cpp
 *
 *  Created on: May 9, 2016
 *      Author: sled
 */

#include "PerimetralConesDetector.h"

PerimetralConesDetector *PerimetralConesDetector::getInstance()
{
	static PerimetralConesDetector *instance = 0;

	if (instance == 0)
	{
		instance = new PerimetralConesDetector();
	}

	return instance;
}

PerimetralConesDetector::PerimetralConesDetector()
{
}

PerimetralConesDetector::~PerimetralConesDetector()
{
}

PerimetralConeSet4 PerimetralConesDetector::process_data_8UC4(uint8_t *data, int width, int height)
{
	PerimetralConeSet4 res;
	Rect r[4];
	Mat frame_8UC4(Size(width, height, CV_8UC1, data));
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours(frame_8UC4, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<vector<Point>> contours_poly(contours.size());
	vector<float>radius(contours.size());

	for (int i = 0; i < 4; i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		r[i] = boundingRect(Mat(contours_poly[i]));
		rectangle(frame_8UC4, r[i], Scalar(255, 0, 0, 255), 2);
	}

	res.topLeft 	= r[0];
	res.bottomLeft	= r[1];
	res.bottomRight = r[2];
	res.topRight 	= r[3];

	return res;
}



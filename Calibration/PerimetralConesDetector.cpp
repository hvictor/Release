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

bool PerimetralConesDetector::valueInRange(int value, int min, int max)
{
    return (value >= min-30) && (value <= max+30);
}


bool PerimetralConesDetector::perimeter_overlap(Rect A, Rect B)
{
    bool xOverlap = valueInRange(A.tl().x, B.tl().x, B.br().x) || valueInRange(B.tl().x, A.tl().x, A.br().x);
    bool yOverlap = valueInRange(A.tl().y, B.tl().y, B.br().y) || valueInRange(B.tl().y, A.tl().y, A.br().y);
    printf("OVERLAP\n");

    return xOverlap && yOverlap;
}

Rect PerimetralConesDetector::merge_perimeters(vector<Rect> regions)
{
    unsigned int xmin = regions[0].tl().x;
    unsigned int xmax = regions[0].br().x;
    unsigned int ymin = regions[0].tl().y;
    unsigned int ymax = regions[0].br().y;

    for (int i = 1; i < regions.size(); i++) {
        if (regions[i].tl().x < xmin) xmin = regions[i].tl().x;
        if (regions[i].br().x > xmax) xmax = regions[i].br().x;
        if (regions[i].tl().y < ymin) ymin = regions[i].tl().y;
        if (regions[i].br().y > ymax) ymax = regions[i].br().y;
    }

    return Rect(Point(xmin, ymin), Point(xmax, ymax));
}


PerimetralConeSet4 PerimetralConesDetector::process_data_8UC4(uint8_t *data, int width, int height, bool *status)
{
	PerimetralConeSet4 res;
	vector<Rect> rs;
	Mat frame_8UC4(Size(width, height), CV_8UC4, data);
	Mat frame_8UC1(Size(width, height), CV_8UC1);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	cvtColor(frame_8UC4, frame_8UC1, CV_RGBA2GRAY);
	findContours(frame_8UC1, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<vector<Point>> contours_poly(contours.size());
	vector<float>radius(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		Rect r = boundingRect(Mat(contours_poly[i]));
		rs.push_back(r);
		//rectangle(frame_8UC4, r, Scalar(255, 0, 0, 255), 2);
		//cout << "Cone: " << r << endl;
	}

	while (true) {
		bool merged_perim = false;

		for (vector<Rect>::iterator it = rs.begin(); it != rs.end(); ) {
			Rect i = *it;
			vector<Rect> contacts;

			// Building contacting regions list
			for (vector<Rect>::iterator jt = it+1; jt != rs.end(); ) {
				Rect j = *jt;

				if (perimeter_overlap(i, j)) {
					contacts.push_back(j);
					jt = rs.erase(jt);
				}

				if (jt != rs.end()) jt++;
			}

			// Merge region i with all contacting regions, and replace this new region in the original list
			if (contacts.size() > 0) {
				contacts.push_back(i);
				Rect m = merge_perimeters(contacts);
				// Here only m exists: memory of i and <contacts> has been freed
				it = rs.erase(it);
				rs.push_back(m);
				merged_perim = true;
			}

			if (it != rs.end()) it++;
		}

		if (!merged_perim)
			break;
	}

	for (int i = 0; i < rs.size(); i++)
	{
		rectangle(frame_8UC4, rs[i], Scalar(255, 0, 0, 255), 1);
	}

	if (rs.size() != 4) {
		*status = false;
		return res;
	}

	int tl_index;
	int br_index;
	int tr_index;

	// Search Top Left cone perimeter
	res.topLeft = rs[0];
	for (int i = 1; i < 4; i++) {
		if (rs[i].tl().x < res.topLeft.x && rs[i].tl().y < res.topLeft.y) {
			res.topLeft = rs[i];
			tl_index = i;
		}
	}

	// Search Bottom Right cone perimeter
	res.bottomRight = rs[0];
	for (int i = 1; i < 4; i++) {
		if (rs[i].br().x > res.bottomRight.x && rs[i].br().y > res.bottomRight.y) {
			res.bottomRight = rs[i];
			br_index = i;
		}
	}

	// Top Right is the remaining area with smaller y

	bool tr_set = false;
	for (int i = 0; i < 4; i++) {
		if (i == tl_index || i == br_index)
			continue;

		if (!tr_set) {
			res.topRight = rs[i];
			tr_set = true;
		}
		else if (rs[i].y < res.topRight) {
			res.topRight = rs[i];
			tr_index = i;
		}
	}

	// The remaining one is Bottom Left
	for (int i = 0; i < 4; i++) {
		if (i == tl_index || i == br_index || i == tr_index)
			continue;

		res.bottomLeft = rs[i];
	}

	*status = true;

	rectangle(frame_8UC4, res.topLeft, Scalar(255, 255, 0, 255), 1);
	rectangle(frame_8UC4, res.topRight, Scalar(255, 255, 0, 255), 1);
	rectangle(frame_8UC4, res.bottomLeft, Scalar(255, 255, 0, 255), 1);
	rectangle(frame_8UC4, res.bottomRight, Scalar(255, 255, 0, 255), 1);

	return res;
}



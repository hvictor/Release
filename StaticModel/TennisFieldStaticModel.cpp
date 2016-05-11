/*
 * TennisFieldStaticModel.cpp
 *
 *  Created on: Jan 29, 2016
 *      Author: sled
 */

#include "TennisFieldStaticModel.h"

TennisFieldStaticModel *TennisFieldStaticModel::getInstance()
{
	static TennisFieldStaticModel *instance = 0;

	if (instance == 0) {
		instance = new TennisFieldStaticModel();
	}

	return instance;
}

TennisFieldStaticModel::TennisFieldStaticModel():
		fieldDelimiter(0),
		_score(0),
		_line(0)
{
}

int TennisFieldStaticModel::getScore()
{
	return _score;
}

int TennisFieldStaticModel::getLineHits()
{
	return _line;
}

TennisFieldStaticModel::~TennisFieldStaticModel()
{
}

void TennisFieldStaticModel::setTennisFieldDelimiter(TennisFieldDelimiter *fieldDelimiter)
{
	this->fieldDelimiter = fieldDelimiter;
}

void TennisFieldStaticModel::computeTennisFieldVisualPoints(Mat objectPoints, Mat& visualPoints3D, double h_displacement)
{
	Point3f near_L = objectPoints.at<Vec3f>(0, 0);
	Point3f near_R = objectPoints.at<Vec3f>(0, 1);
	Point3f far_R = objectPoints.at<Vec3f>(0, 2);
	Point3f far_L = objectPoints.at<Vec3f>(0, 3);

	float deltax = (near_R.x - near_L.x) / 10;

	float deltaz = (far_L.z - near_L.z) / 15;

	float deltad = h_displacement / 15;

	int pos = 0;
	for (int z = 0; z < 15; z++) {
		for (int x = 0; x <= 10; x++) {
			visualPoints3D.at<Vec3f>(0, pos++) = Point3f(near_L.x + x*deltax, near_L.y + deltad*z, near_L.z + z*deltaz);
		}
	}
}

void TennisFieldStaticModel::computeTennisNetVisualPoints(Mat objectPoints, Mat& visualPoints3D, double h_displacement)
{
	Point3f near_L = objectPoints.at<Vec3f>(0, 0);
	Point3f near_U = objectPoints.at<Vec3f>(0, 1);
	Point3f far_U = objectPoints.at<Vec3f>(0, 2);
	Point3f far_L = objectPoints.at<Vec3f>(0, 3);

	float deltay = (near_L.y - near_U.y) / 10;

	float deltaz = (far_U.z - near_U.z) / 15;

	float deltad = h_displacement / 15;

	int pos = 0;
	for (int z = 0; z < 10; z++) {
		for (int y = 0; y <= 10; y++) {
			visualPoints3D.at<Vec3f>(0, pos++) = Point3f(near_L.x, near_U.y + y*deltay + deltad*z, near_L.z + z*deltaz);
		}
	}
}

int TennisFieldStaticModel::checkTennisField_2D(TennisField_3D *field, double x, double y)
{
/*
	bool found = false;
	for (int i = 0; i < field->score_positions.size(); i++) {
		if (field->score_positions[i].x == (float)x && field->score_positions[i].y == (float)y) {
			found = true;
			break;
		}
	}

	if (found)
		return -1;
*/

	vector<Point2f> vertices;
	vertices.push_back(field->near_L);
	vertices.push_back(field->near_R);
	vertices.push_back(field->far_R);
	vertices.push_back(field->far_L);
	int c = (int)pointPolygonTest(vertices, Point2f((float)x, (float)y), false);

	if (c > 0) {
		field->score_positions.push_back(Point2f((float)x, (float)y));
		field->score++;
		field->trigger = true;
	}
	else if (c == 0) {
		field->nets++;
	}

	return c;
}

int TennisFieldStaticModel::updateScoreTracking_2D(double x, double y)
{
	vector<Point2f> vertices;
	vertices.push_back(fieldDelimiter->topLeft);
	vertices.push_back(fieldDelimiter->topRight);
	vertices.push_back(fieldDelimiter->bottomLeft);
	vertices.push_back(fieldDelimiter->bottomRight);
	int c = (int)pointPolygonTest(vertices, Point2f((float)x, (float)y), false);

	if (c > 0) {
		score_positions.push_back(Point2f((float)x, (float)y));
		_score++;
	}
	else if (c == 0) {
		_line++;
	}

	return c;
}

TennisFieldDelimiter *TennisFieldStaticModel::getTennisFieldDelimiter()
{
	return fieldDelimiter;
}

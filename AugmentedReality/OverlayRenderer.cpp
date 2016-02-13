/*
 * OverlayRenderer.cpp
 *
 *  Created on: Feb 3, 2016
 *      Author: sled
 */

#include "OverlayRenderer.h"

OverlayRenderer *OverlayRenderer::getInstance()
{
	static OverlayRenderer *instance = 0;

	if (!instance) {
		instance = new OverlayRenderer();
	}

	return instance;
}

OverlayRenderer::OverlayRenderer() {
}

OverlayRenderer::~OverlayRenderer() {
}

void OverlayRenderer::renderTracker(Mat frame, Point p, int size)
{
	rectangle(frame, Point(p.x - size, p.y - size), Point(p.x + size, p.y + size), Scalar(0, 0, 255), 1);
	line(frame, Point(p.x, p.y - size), Point(p.x, p.y - (size-3)), Scalar(0, 0, 255), 1);
	line(frame, Point(p.x, p.y + size), Point(p.x, p.y + (size-3)), Scalar(0, 0, 255), 1);
	line(frame, Point(p.x - size, p.y), Point(p.x - (size-3), p.y), Scalar(0, 0, 255), 1);
	line(frame, Point(p.x + (size-3), p.y), Point(p.x + size, p.y), Scalar(0, 0, 255), 1);
}

void OverlayRenderer::renderTennisNet(Mat frame, Mat netPoints, Mat netVisualPoints)
{
	Vec2f image_coords0 = netPoints.at<Vec2f>(0, 0);
	Vec2f image_coords1 = netPoints.at<Vec2f>(0, 1);
	Vec2f image_coords2 = netPoints.at<Vec2f>(0, 2);
	Vec2f image_coords3 = netPoints.at<Vec2f>(0, 3);

	// Net image coordinates
	Point2f p0(image_coords0.val[0], image_coords0.val[1]);
	Point2f p1(image_coords1.val[0], image_coords1.val[1]);
	Point2f p2(image_coords2.val[0], image_coords2.val[1]);
	Point2f p3(image_coords3.val[0], image_coords3.val[1]);

	// Rendering visual points

	Scalar color(50, 255, 50);

	for (int i = 0; i < 11*10; i++) {
		Point2f visualPoint(netVisualPoints.at<Vec2f>(0, i).val[0], netVisualPoints.at<Vec2f>(0, i).val[1]);
		circle(frame, visualPoint, 1, color, -1);
		if (!(i % 15)) {
			if (color.val[0] + 40 > 255) color.val[0] = 255;
			else {
				color.val[0] += 40;
				color.val[0] -= 20;
			}
		}
	}

	line(frame, p0, p1,  cvScalar(255, 0, 0), 2);
	line(frame, p1, p2,  cvScalar(255, 0, 0), 2);
	line(frame, p2, p3,  cvScalar(255, 0, 0), 2);
	line(frame, p3, p0,  cvScalar(255, 0, 0), 2);
}

void OverlayRenderer::renderTennisField(TennisField_3D *field, Mat frame, Mat imageFieldBoundaries, Mat imageVisualPoints)
{
	Vec2f image_coords0 = imageFieldBoundaries.at<Vec2f>(0, 0);
	Vec2f image_coords1 = imageFieldBoundaries.at<Vec2f>(0, 1);
	Vec2f image_coords2 = imageFieldBoundaries.at<Vec2f>(0, 2);
	Vec2f image_coords3 = imageFieldBoundaries.at<Vec2f>(0, 3);

	// Ground image coordinates
	Point2f p0(image_coords0.val[0], image_coords0.val[1]);
	Point2f p1(image_coords1.val[0], image_coords1.val[1]);
	Point2f p2(image_coords2.val[0], image_coords2.val[1]);
	Point2f p3(image_coords3.val[0], image_coords3.val[1]);

	field->near_L = p0;
	field->near_R = p1;
	field->far_R = p2;
	field->far_L = p3;

	line(frame, p0, p1,  cvScalar(0, 255, 0), 1);
	line(frame, p1, p2,  cvScalar(0, 255, 0), 1);
	line(frame, p2, p3,  cvScalar(0, 255, 0), 1);
	line(frame, p3, p0,  cvScalar(0, 255, 0), 1);

	// Rendering visual points

	Scalar color(0, 255, 255);

	for (int i = 0; i < 11*15; i++) {
		Point2f visualPoint(imageVisualPoints.at<Vec2f>(0, i).val[0], imageVisualPoints.at<Vec2f>(0, i).val[1]);
		circle(frame, visualPoint, 2.0, color, -1);
		if (!(i % 15)) color.val[1] -= 40;
	}
}

void OverlayRenderer::renderGPUDeviceMemoryProgressBar(Mat& frame, float memused_percent)
{
	rectangle(frame, Point(360, 22), Point(460, 27), Scalar(50, 205, 50), 1);
	rectangle(frame, Point(360, 22), Point(360 + memused_percent, 27), Scalar(50, 205, 50), -1);
}

void OverlayRenderer::renderGPUDeviceStatus(Mat& frame)
{
	DeviceInfo device_info(0);
	stringstream s;
	stringstream t;

	float memused = (((float)device_info.totalMemory() - (double)device_info.freeMemory())/ (double)device_info.totalMemory()) * 100.0;

	s << "GPU: " << device_info.name() << "    Mem: " << memused << "%";

	putText(frame, s.str(), cvPoint(10,10), FONT_HERSHEY_SIMPLEX, 0.2, cvScalar(50, 205, 50), 1, CV_AA);

	renderGPUDeviceMemoryProgressBar(frame, memused);
}

void OverlayRenderer::renderModelInformation(Mat& frame, vector<Trajectory *> trajectories, char cameraFlag)
{
	int y = 12;

	stringstream s;

	s << "[Optical Layer] Quadratic Model " << cameraFlag;
	putText(frame, s.str(), cvPoint(1, y), FONT_HERSHEY_SIMPLEX, 0.3, cvScalar(50, 205, 50), 1, CV_AA);
	s.str(std::string());
	y += 11;

	s << "FILT: Low-Pass";
	putText(frame, s.str(), cvPoint(1, y), FONT_HERSHEY_SIMPLEX, 0.3, cvScalar(50, 205, 50), 1, CV_AA);
	s.str(std::string());
	y += 11;

	line(frame, Point(1, y + 3), Point(140, y + 3),  cvScalar(50, 205, 50), 1);
	y += 3;

	s << "TRAJ: " << trajectories.size() << " active";
	s.str(std::string());
	y += 11;

	for (int i = 0; i < trajectories.size(); i++) {
		s << "T" << i << ": Vx=" << trajectories[i]->velocityX << " Vy=" << trajectories[i]->velocityY;
		putText(frame, s.str(), cvPoint(1, y), FONT_HERSHEY_SIMPLEX, 0.3, cvScalar(50, 205, 50), 1, CV_AA);
		s.str(std::string());
		y += 11;
	}

	// Write impact status
	s.str(std::string());
	s << "[IMPACT]";
	bool renderImpactStatus = false;
	for (int i = 0; i < trajectories.size(); i++) {
		bool impacted = false;
		for (int j = trajectories[i]->motionFlow.size()-8; j < trajectories[i]->motionFlow.size(); j++) {
			MovingObject *obj = trajectories[i]->motionFlow[j];
			if (obj->impacted) {
				impacted = true;
				break;
			}
		}

		if (impacted) {
			s << " T" << i;
			renderImpactStatus = true;
		}
	}

	if (renderImpactStatus) putText(frame, s.str(), cvPoint(1, 234), FONT_HERSHEY_SIMPLEX, 0.3, cvScalar(0, 255, 255), 1, CV_AA);

	// Augment frame with score information
	/* TODO: Legacy
	char score[30];
	sprintf(score, "[SCORE] P1: %d - P2: %d", field1.score, field2.score);
	putText(frame, const_cast<char *>(score), cvPoint(200, 12), FONT_HERSHEY_SIMPLEX, 0.3, cvScalar(0, 255, 255), 1, CV_AA);
	*/
}

void OverlayRenderer::renderArrow(Mat dst, Point S, Point P, Scalar predictionColor, double versorX, double versorY)
{
	double size = 3.0;
	double normalX = -versorY;
	double normalY = versorX;

	Point points[1][3];
	points[0][2] = Point(P.x + 1.0 * size, P.y + 1.0 * size);
	points[0][0] = Point(P.x + normalX*size, P.y + normalY*size);
	points[0][1] = Point(P.x - normalX*size, P.y - normalY*size);

	line(dst, S, P, predictionColor, 1.0);
	const Point* ppt[1] = { points[0] };
	int npt[] = { 3 };
	fillPoly(dst, ppt, npt, 1, predictionColor, 8);
}

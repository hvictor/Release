/*
 * OverlayRenderer.cpp
 *
 *  Created on: Feb 3, 2016
 *      Author: sled
 */

#include "OverlayRenderer.h"
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

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

void OverlayRenderer::renderStatus_8UC4(uint8_t *u8data, int width, int height, char *statusMessage, Scalar color)
{
	Mat frame_RGBA(Size(width, height), CV_8UC4, u8data);
	putText(frame_RGBA, statusMessage, Point(10, 30), FONT_HERSHEY_SIMPLEX, 1.0, color, 2, CV_AA);
}

void OverlayRenderer::renderFieldDelimiter_8UC4(uint8_t *u8data, int width, int height, TennisFieldDelimiter *fieldDelimiter)
{
	Mat frame_RGBA(Size(width, height), CV_8UC4, u8data);

	line(frame_RGBA, fieldDelimiter->topLeft, fieldDelimiter->topRight, Scalar(255, 0, 0, 255), 2);
	line(frame_RGBA, fieldDelimiter->topRight, fieldDelimiter->bottomRight, Scalar(255, 0, 0, 255), 2);
	line(frame_RGBA, fieldDelimiter->bottomRight, fieldDelimiter->bottomLeft, Scalar(255, 0, 0, 255), 2);
	line(frame_RGBA, fieldDelimiter->bottomLeft, fieldDelimiter->topLeft, Scalar(255, 0, 0, 255), 2);
}

void OverlayRenderer::renderFieldDelimiter_Mat8UC4(Mat frame_RGBA, TennisFieldDelimiter *fieldDelimiter)
{
	if (fieldDelimiter == 0)
		return;

	line(frame_RGBA, fieldDelimiter->topLeft, fieldDelimiter->topRight, Scalar(0, 255, 0, 255), 3);
	line(frame_RGBA, fieldDelimiter->topRight, fieldDelimiter->bottomRight, Scalar(0, 255, 0, 255), 3);
	line(frame_RGBA, fieldDelimiter->bottomRight, fieldDelimiter->bottomLeft, Scalar(0, 255, 0, 255), 3);
	line(frame_RGBA, fieldDelimiter->bottomLeft, fieldDelimiter->topLeft, Scalar(0, 255, 0, 255), 3);
}

void OverlayRenderer::renderInterpolatedTrajectoryCubic(Mat frame, TrajectoryDescriptor *descriptor)
{
	Point pi, pj;
	vector<TrajectorySection *> tSect = descriptor->getTrajectorySections();

	for (int i = 0; i < tSect.size(); i++) {
		CubicPolynomialCoeff C = tSect[i]->coeffs;
		double x0 = tSect[i]->x_from;
		double x1 = tSect[i]->x_to;

		for (int x = x0; x <= x1; x++) {
			double y = C.c0 + C.c1*x + C.c2*x*x + C.c3*x*x*x;

			if (x == x0) {
				pj = Point((int)x, (int)y);
				continue;
			}

			pi = pj;
			pj = Point((int)x, (int)y);

			line(frame, pi, pj, OVERLAY_COLOR_YELLOW, 1);
		}
	}
}

void OverlayRenderer::renderInterpolatedTrajectoryHexa(Mat frame, TrajectoryDescriptor *descriptor)
{
	Point pi, pj;
	vector<TrajectorySection *> tSect = descriptor->getTrajectorySections();

	if (descriptor->getID() != 6) {
		printf("Descriptor not 6, leaving rendering (DISABLE ME)\n");
		return;
	}

	for (int i = 0; i < tSect.size(); i++) {
		if (tSect[i]->optimized) {
			CubicPolynomialCoeff C = tSect[i]->coeffs;
			double x0 = tSect[i]->x_from;
			double x1 = tSect[i]->x_to;

			for (int x = x0; x <= x1; x++) {
				double y = C.c0 + C.c1*x + C.c2*x*x + C.c3*x*x*x;

				if (x == x0) {
					pj = Point((int)x, (int)y);
					continue;
				}

				pi = pj;
				pj = Point((int)x, (int)y);
				line(frame, pi, pj, OVERLAY_COLOR_ORANGE, 2);

				if (x == x0+1) {
					rectangle(frame, Point((int)(x-6), (int)(y-6)), Point((int)(x+6), (int)(y+6)), OVERLAY_COLOR_YELLOW, 1);
				}
			}
		}
		else {
			HexaPolynomialCoeff C = tSect[i]->hexa_coeffs;
			double x0 = tSect[i]->x_from;
			double x1 = tSect[i]->x_to;

			for (int x = x0; x <= x1; x++) {
				double y = C.c0 + C.c1*x + C.c2*x*x + C.c3*x*x*x + C.c4*x*x*x*x + C.c5*x*x*x*x*x + C.c6*x*x*x*x*x*x;

				if (x == x0) {
					pj = Point((int)x, (int)y);
					continue;
				}

				pi = pj;
				pj = Point((int)x, (int)y);

				line(frame, pi, pj, OVERLAY_COLOR_YELLOW, 1);
			}
		}
	}
}

void OverlayRenderer::renderTracker(Mat frame, Point p, int size)
{
	rectangle(frame, Point(p.x - size, p.y - size), Point(p.x + size, p.y + size), OVERLAY_COLOR_GREEN, 1);
	line(frame, Point(p.x, p.y - size), Point(p.x, p.y - (size-3)), OVERLAY_COLOR_GREEN, 1);
	line(frame, Point(p.x, p.y + size), Point(p.x, p.y + (size-3)), OVERLAY_COLOR_GREEN, 1);
	line(frame, Point(p.x - size, p.y), Point(p.x - (size-3), p.y), OVERLAY_COLOR_GREEN, 1);
	line(frame, Point(p.x + (size-3), p.y), Point(p.x + size, p.y), OVERLAY_COLOR_GREEN, 1);
}

void OverlayRenderer::renderTrackerState(Mat frame, StateRelatedTable *table, Point p)
{
	char stateMessage[60];
	sprintf(stateMessage, "[%d]", table->stateTableID);
	putText(frame, stateMessage, Point(p.x-10, p.y-15), FONT_HERSHEY_SIMPLEX, 0.4, OVERLAY_COLOR_GREEN, 1, CV_AA);
}

void OverlayRenderer::renderHumanTrackers(Mat frame, vector<cv::Rect> humanFigures)
{
	for(unsigned i = 0; i < humanFigures.size(); i++) {
		char buffer[60];
		sprintf(buffer, "PLAYER %d", i);
		cv::Rect r = humanFigures[i];

		int len = r.width/4;

		line(frame, r.tl(), Point(r.tl().x, r.tl().y + len), OVERLAY_COLOR_GREEN, 2);
		line(frame, r.tl(), Point(r.tl().x + len, r.tl().y), OVERLAY_COLOR_GREEN, 2);

		line(frame, Point(r.tl().x + r.width - len, r.tl().y), Point(r.tl().x + r.width, r.tl().y), OVERLAY_COLOR_GREEN, 2);
		line(frame, Point(r.tl().x + r.width, r.tl().y), Point(r.tl().x + r.width, r.tl().y + len), OVERLAY_COLOR_GREEN, 2);

		line(frame, r.br(), Point(r.br().x, r.br().y - len), OVERLAY_COLOR_GREEN, 2);
		line(frame, r.br(), Point(r.br().x - len, r.br().y), OVERLAY_COLOR_GREEN, 2);

		line(frame, Point(r.tl().x + len, r.br().y), Point(r.tl().x, r.br().y), OVERLAY_COLOR_GREEN, 1);
		line(frame, Point(r.tl().x, r.br().y), Point(r.tl().x, r.br().y - len), OVERLAY_COLOR_GREEN, 1);

		putText(frame, buffer, Point(r.tl().x, r.tl().y - 15), FONT_HERSHEY_SIMPLEX, 1.0, OVERLAY_COLOR_GREEN, 2, CV_AA);
	}
}

void OverlayRenderer::renderPerimetralConeSet4_8UC4(uint8_t *u8data, int width, int height, PerimetralConeSet4 cones_set)
{
	Mat frame_8UC4(Size(width, height), CV_8UC4, u8data);

	line(frame_8UC4, cones_set.vertex_topLeft, cones_set.vertex_topRight, Scalar(0, 255, 0, 255), 2);
	line(frame_8UC4, cones_set.vertex_topRight, cones_set.vertex_bottomRight, Scalar(0, 255, 0, 255), 2);
	line(frame_8UC4, cones_set.vertex_bottomRight, cones_set.vertex_bottomLeft, Scalar(0, 255, 0, 255), 2);
	line(frame_8UC4, cones_set.vertex_bottomLeft, cones_set.vertex_topLeft, Scalar(0, 255, 0, 255), 2);
}

void OverlayRenderer::renderTargetTracker(Mat frame, Point center)
{
		char buffer[60];
		strcpy(buffer, "TARGET");

		Rect r(center.x-10, center.y-10, 30, 30);

		int len = r.width/4;

		line(frame, r.tl(), Point(r.tl().x, r.tl().y + len), OVERLAY_COLOR_BLUE_RGBA, 2);
		line(frame, r.tl(), Point(r.tl().x + len, r.tl().y), OVERLAY_COLOR_BLUE_RGBA, 2);

		line(frame, Point(r.tl().x + r.width - len, r.tl().y), Point(r.tl().x + r.width, r.tl().y), OVERLAY_COLOR_BLUE_RGBA, 2);
		line(frame, Point(r.tl().x + r.width, r.tl().y), Point(r.tl().x + r.width, r.tl().y + len), OVERLAY_COLOR_BLUE_RGBA, 2);

		line(frame, r.br(), Point(r.br().x, r.br().y - len), OVERLAY_COLOR_BLUE_RGBA, 2);
		line(frame, r.br(), Point(r.br().x - len, r.br().y), OVERLAY_COLOR_BLUE_RGBA, 2);

		line(frame, Point(r.tl().x + len, r.br().y), Point(r.tl().x, r.br().y), OVERLAY_COLOR_BLUE_RGBA, 2);
		line(frame, Point(r.tl().x, r.br().y), Point(r.tl().x, r.br().y - len), OVERLAY_COLOR_BLUE_RGBA, 2);

		putText(frame, buffer, Point(r.tl().x - 30, r.tl().y - 15), FONT_HERSHEY_SIMPLEX, 1.0, OVERLAY_COLOR_BLUE_RGBA, 2, CV_AA);
}

void OverlayRenderer::renderStaticModelScoreTracking(Mat frame, TennisFieldStaticModel *staticModel)
{
	char scoreBuf[100];

	sprintf(scoreBuf, "SCORE: %d        LINE HIT: %d", staticModel->getScore(), staticModel->getLineHits());
	putText(frame, scoreBuf, Point(10, 30), FONT_HERSHEY_SIMPLEX, 1.0, OVERLAY_COLOR_GREEN_RGBA, 2, CV_AA);
}

void OverlayRenderer::renderPredatorState(Mat frame, TargetPredator *tgtPredator)
{
	list<pred_state_t> *predator_state = tgtPredator->get_state();

	int i = 0;

	Scalar color = Scalar(255, 255, 0, 0);

	for (list<pred_state_t>::iterator it = predator_state->begin(); it != predator_state->end(); )
	{
		if (i >= 10)
			break;

		int interp = 4;

		int x0 = it->display_x;
		int y0 = it->display_y;

		bool impact_status = it->impact_status;

		it++;

		if (it == predator_state->end())
			break;

		int x1 = it->display_x;
		int y1 = it->display_y;

		double Vx = (x1 - x0);
		double Vy = (y1 - y0);

		for (int k = 0; k < interp; k++) {
			double cx = (double)x0 + ((double)(Vx * k) / (double)interp);
			double cy = (double)y0 + ((double)(Vy * k) / (double)interp);

			if (impact_status && k == 0) {
				rectangle(frame, Point2f(x1 - 10.0, y1 - 10.0), Point2f(x1 + 10.0, y1 + 10.0), OVERLAY_COLOR_GREEN_RGBA, 2);
				//putText(frame, "[IMPACT]", Point(10, 30), FONT_HERSHEY_SIMPLEX, 1.0, OVERLAY_COLOR_GREEN_RGBA, 2, CV_AA);
			}
			else {
				color.val[1] -= 5;
				color.val[3] -= 5;
				rectangle(frame, Point2f(cx - 4.0, cy - 4.0), Point2f(cx + 4.0, cy + 4.0), color, 1);
			}
		}

		if (i == 0) {
			// The first list element is the latest: Predator performs list::push_front on new data.
			// So, apart from movement trace rendering, the true velocity vector between the latest data and the immediately
			// previous one is: [-Vx, -Vy]. The vector must be rendered starting from x0,y0, the tracking head.

			//renderArrow(frame, Point(x0, y0), Point(x0 - Vx, y0 - Vy));
		}

		i++;
	}
}

void OverlayRenderer::renderPredatorTrackingWnd(Mat frame, pred_wnd_t predTrackingWnd)
{
	rectangle(frame, Point(predTrackingWnd.cx - predTrackingWnd.w/2, predTrackingWnd.cy - predTrackingWnd.h/2),
			Point(predTrackingWnd.cx + predTrackingWnd.w/2, predTrackingWnd.cy + predTrackingWnd.h/2), OVERLAY_COLOR_YELLOW_RGBA, 2);
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

void OverlayRenderer::renderArrow(Mat frame, Point p, Point q)
{

	Scalar yellow(255, 255, 0, 255);
	double angle = atan2((double) p.y - q.y, (double) p.x - q.x);

	line(frame, p, q, yellow, 2);

	p.x = (int) (q.x + 9 * cos(angle + CV_PI / 4));
	p.y = (int) (q.y + 9 * sin(angle + CV_PI / 4));

	line(frame, p, q, yellow, 2);

	p.x = (int) (q.x + 9 * cos(angle - CV_PI / 4));
	p.y = (int) (q.y + 9 * sin(angle - CV_PI / 4));

	line(frame, p, q, yellow, 2);
}

void OverlayRenderer::renderTarget3DPosition(Mat frame, Point center, StereoSensorMeasure3D measure, float confidence)
{
	char stateMessage[200];

	measure.x_mm /= 1000.0;
	measure.y_mm /= 1000.0;
	measure.z_mm /= 1000.0;

	printf("diooo\n");

	sprintf(stateMessage, "T=[%d,%d] XYZ=[%.2f, %.2f, %.2f] CONFID=%.2f", center.x, center.y, measure.x_mm, measure.y_mm, measure.z_mm, confidence);

	printf("diooooooo\n");
	putText(frame, stateMessage, Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.7, OVERLAY_COLOR_RED_RGBA, 2, CV_AA);

	printf("gesu\n");
	circle(frame, center, 5, OVERLAY_COLOR_RED_RGBA, -1);

	printf("madonna\n");
}

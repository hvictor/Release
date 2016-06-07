/*
 * OverlayRenderer.h
 *
 *  Created on: Feb 3, 2016
 *      Author: sled
 */

#ifndef OVERLAYRENDERER_H_
#define OVERLAYRENDERER_H_

#include "../Common/opencv_headers.h"
#include "../StaticModel/TennisField3D.h"
#include "../OpticalLayer/TrajectoryRecognizer.h"
#include "../OpticalLayer/StatefulObjectFilter.h"
#include "../TrajectoryTracking/TrajectoryDescriptor.h"
#include "../TargetPredator/TargetPredator.h"
#include "../Calibration/PerimetralConesDetector.h"
#include "../Calibration/TennisFieldDelimiter.h"
#include "../SensorAbstractionLayer/StereoSensorAbstractionLayer.h"
#include "../StaticModel/GroundModel.h"
#include "../StaticModel/NetModel.h"
#include <stdint.h>

#define OVERLAY_COLOR_GREEN			Scalar(50, 205, 50)
#define OVERLAY_COLOR_GREEN_RGBA	Scalar(50, 205, 50, 255)
#define OVERLAY_COLOR_BLUE_RGBA		Scalar(0, 0, 255, 255)
#define OVERLAY_COLOR_RED_RGBA		Scalar(255, 0, 0, 255)
#define OVERLAY_COLOR_RED			Scalar(0, 0, 255)
#define OVERLAY_COLOR_YELLOW		Scalar(0, 255, 255)
#define OVERLAY_COLOR_YELLOW_RGBA	Scalar(255, 255, 0, 255)
#define OVERLAY_COLOR_ORANGE		Scalar(0, 200, 255)
#define OVERLAY_COLOR_ORANGE_RGBA	Scalar(255, 200, 0, 255)

class OverlayRenderer {
public:
	static OverlayRenderer *getInstance();
	void renderTennisNet(Mat frame, Mat netPoints, Mat netVisualPoints);
	void renderTennisField(TennisField_3D *field, Mat frame, Mat imageFieldBoundaries, Mat imageVisualPoints);
	void renderGPUDeviceMemoryProgressBar(Mat& frame, float memused_percent);
	void renderGPUDeviceStatus(Mat& frame);
	void renderModelInformation(Mat& frame, vector<Trajectory *> trajectories, char cameraFlag);
	void renderArrow(Mat dst, Point S, Point P, Scalar predictionColor, double versorX, double versorY);
	void renderArrow(Mat frame, Point p, Point q);
	void renderTracker(Mat frame, Point p, int size);
	void renderTrackerState(Mat frame, StateRelatedTable *table, Point p);
	void renderHumanTrackers(Mat frame, vector<cv::Rect> humanFigures);
	void renderTargetTracker(Mat frame, Point center);
	void renderTarget3DPosition(Mat frame, Point center, StereoSensorMeasure3D measure);
	void renderPredatorState(Mat frame, TargetPredator *tgtPredator);
	void renderPredatorTrackingWnd(Mat frame, pred_wnd_t predTrackingWnd);
	void renderInterpolatedTrajectoryCubic(Mat frame, TrajectoryDescriptor *descriptor);
	void renderInterpolatedTrajectoryHexa(Mat frame, TrajectoryDescriptor *descriptor);
	void renderStatus_8UC4(uint8_t *u8data, int width, int height, char *statusMessage, Scalar color);
	void renderFieldDelimiter_8UC4(uint8_t *u8data, int width, int height, TennisFieldDelimiter *fieldDelimiter);
	void renderFieldDelimiter_Mat8UC4(Mat frame_RGBA, TennisFieldDelimiter *fieldDelimiter);
	void renderPerimetralConeSet4_8UC4(uint8_t *u8data, int width, int height, PerimetralConeSet4 cones_set);
	void renderStaticModelScoreTracking(Mat frame, TennisFieldStaticModel *staticModel);
	void renderDepthInformation(Mat frame, int x, int y, float depth);
	void renderPlanePointSetProjection_8UC4(uint8_t *u8data, int width, int height, Point2f nearL, Point2f nearR, Point2f farR, Point2f farL);
	void renderFloorPlane_8UC4(uint8_t *u8data, int width, int height, GroundModel *model);
	void renderImpactData3D(Mat frame, Vector3D impact_pos);
	void renderPlaneReferenceSystemAxis_8UC4(uint8_t *u8data, int width, int height, PlaneReferenceSystemAxis refSysAxis);
	void renderNet_8UC4(uint8_t *u8data, int width, int height, NetVisualProjection netVisualProjection);
	void renderNetVisualPoints_8UC4(uint8_t *u8data, int width, int height, vector<Point2f> netVisualPoints);
	void renderNet(Mat frame_RGBA, NetVisualProjection netVisualProjection);

private:
	OverlayRenderer();
	virtual ~OverlayRenderer();
};

#endif /* OVERLAYRENDERER_H_ */

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

#define OVERLAY_COLOR_GREEN			Scalar(50, 205, 50)
#define OVERLAY_COLOR_RED			Scalar(0, 0, 255)
#define OVERLAY_COLOR_YELLOW		Scalar(0, 255, 255)

class OverlayRenderer {
public:
	static OverlayRenderer *getInstance();
	void renderTennisNet(Mat frame, Mat netPoints, Mat netVisualPoints);
	void renderTennisField(TennisField_3D *field, Mat frame, Mat imageFieldBoundaries, Mat imageVisualPoints);
	void renderGPUDeviceMemoryProgressBar(Mat& frame, float memused_percent);
	void renderGPUDeviceStatus(Mat& frame);
	void renderModelInformation(Mat& frame, vector<Trajectory *> trajectories, char cameraFlag);
	void renderArrow(Mat dst, Point S, Point P, Scalar predictionColor, double versorX, double versorY);
	void renderTracker(Mat frame, Point p, int size);
	void renderTrackerState(Mat frame, StateRelatedTable *table, Point p);
	void renderHumanTrackers(Mat frame, vector<cv::Rect> humanFigures);
	void renderInterpolatedTrajectory(Mat frame, TrajectoryDescriptor *descriptor);

private:
	OverlayRenderer();
	virtual ~OverlayRenderer();
};

#endif /* OVERLAYRENDERER_H_ */

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

class OverlayRenderer {
public:
	static OverlayRenderer *getInstance();
	void renderTennisNet(Mat frame, Mat netPoints, Mat netVisualPoints);
	void renderTennisField(TennisField_3D *field, Mat frame, Mat imageFieldBoundaries, Mat imageVisualPoints);
	void renderGPUDeviceMemoryProgressBar(Mat& frame, float memused_percent);
	void renderGPUDeviceStatus(Mat& frame);
	void renderModelInformation(Mat& frame, vector<Trajectory *> trajectories, char cameraFlag);
	void renderArrow(Mat dst, Point S, Point P, Scalar predictionColor, double versorX, double versorY);
	void OverlayRenderer::renderTracker(Mat frame, Point p, int size);

private:
	OverlayRenderer();
	virtual ~OverlayRenderer();
};

#endif /* OVERLAYRENDERER_H_ */

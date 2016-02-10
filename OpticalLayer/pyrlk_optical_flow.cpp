/*
 * pyrlk_optical_flow.cpp
 *
 *  Created on: Jan 27, 2016
 *      Author: sled
 */

// The following code is mainly taken from the official OpenCV example about the GPU Lucas Kanade optical flow

#include "pyrlk_optical_flow.h"
#include <stdio.h>
#include "../Configuration/configs.h"
#include "../Configuration/Configuration.h"

void download(const GpuMat& d_mat, vector<uchar>& vec)
{
	vec.resize(d_mat.cols);
	Mat mat(1, d_mat.cols, CV_8UC1, (void*)&vec[0]);
	d_mat.download(mat);
}


void download(const GpuMat& d_mat, vector<Point2f>& vec)

{
	vec.resize(d_mat.cols);
	Mat mat(1, d_mat.cols, CV_32FC2, (void*)&vec[0]);
	d_mat.download(mat);
}

bool pointsNotInArea(Point2f p, Point2f q, vector<Rect> discardedImageAreas)
{
	for (int i = 0; i < discardedImageAreas.size(); i++) {
		Rect r = discardedImageAreas[i];
		vector<Point2f> vertices;

		Point2f br(r.br().x, r.br().y);
		Point2f tr(br.x, br.y-r.height);
		Point2f tl(tr.x-r.width, tr.y);
		Point2f bl(tl.x, br.y);
		vertices.push_back(bl);
		vertices.push_back(br);
		vertices.push_back(tr);
		vertices.push_back(tl);

		if (pointPolygonTest(vertices, p, false) >= 0 || pointPolygonTest(vertices, q, false) >= 0) {
			return false;
		}
	}

	return true;
}

vector<FlowObject> getFlowObjects(Mat& frame, const vector<Point2f>& prevPts, const vector<Point2f>& nextPts, const vector<uchar>& status, vector<Rect> discardedImageAreas)
{
	vector<FlowObject> flowObjects;

	for (size_t i = 0; i < prevPts.size(); ++i) {
		Point p = prevPts[i];
		Point q = nextPts[i];

		if (status[i] && pointsNotInArea(p, q, discardedImageAreas)) {

			double displacement_x = p.x - q.x;
			double displacement_y = p.y - q.y;
			double dist = sqrt(displacement_x*displacement_x + displacement_y*displacement_y);

			if (dist < Configuration::getInstance()->getOpticalLayerParameters().lucasKanadeOpticalFlowMinMotion)
				continue;

			FlowObject fwobj;

			fwobj.x = p.x;
			fwobj.y = p.y;
			fwobj.displacement_x = displacement_x;
			fwobj.displacement_y = displacement_y;

			flowObjects.push_back(fwobj);
		}
	}

	return flowObjects;
}

vector<FlowObject> pyrlk_compute(Mat& frame0, Mat& frame1, vector<Rect> discardedImageAreas)
{
	PyrLKOpticalFlow d_pyrLK;
	GoodFeaturesToTrackDetector_GPU detector(1000, 0.01, CFG_PYRLK_MIN_DIST);
	GpuMat d_frame0(frame0);
	GpuMat d_frame1(frame1);

	GpuMat d_prevPts;
	GpuMat d_nextPts;
	GpuMat d_status;

	detector(d_frame0, d_prevPts);
	d_pyrLK.sparse(d_frame0, d_frame1, d_prevPts, d_nextPts, d_status);

	vector<Point2f> prevPts(d_prevPts.cols);
	download(d_prevPts, prevPts);

	vector<Point2f> nextPts(d_nextPts.cols);
	download(d_nextPts, nextPts);

	vector<uchar> status(d_status.cols);
	download(d_status, status);

	return getFlowObjects(frame0, prevPts, nextPts, status, discardedImageAreas);
}

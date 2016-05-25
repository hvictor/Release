/*
 * StereoVision.cpp
 *
 *  Created on: Feb 3, 2016
 *      Author: sled
 */

#include "StereoVision.h"
#include "../DynamicModel3D/DynamicModel3D.h"

StereoVision::StereoVision() {
}

StereoVision::~StereoVision() {
}

vector<Point2f> StereoVision::project3DCoordinatesOnImagePlane(CameraIntrinsics intrinsics, ExtrinsicParameters extrinsics, vector<Vector3D> coords_3d)
{
	Mat objectPoints(1, coords_3d.size(), CV_32FC3);

	for (int i = 0; i < coords_3d.size(); i++) {
		objectPoints.at<Vec3f>(0, i) = Point3f(coords_3d[i].x, coords_3d[i].y, coords_3d[i].z);
	}

	GpuMat d_objectPoints(objectPoints);
	GpuMat d_imagePoints;

	gpu::projectPoints(d_objectPoints, extrinsics.getRotationVector(), extrinsics.getTranslationVector(), intrinsics.cameraMatrix, Mat(), d_imagePoints);

	Mat imagePoints(d_imagePoints);
	vector<Point2f> retval;
	for (int i = 0; i < coords_3d.size(); i++) {
		Vec2f image_coords = imagePoints.at<Vec2f>(0, i);
		Point2f p(image_coords.val[0], image_coords.val[1]);
		retval.push_back(p);
	}

	return retval;
}

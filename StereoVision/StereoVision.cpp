/*
 * StereoVision.cpp
 *
 *  Created on: Feb 3, 2016
 *      Author: sled
 */

#include "StereoVision.h"
#include "../DynamicModel3D/DynamicModel3D.h"
#include "../Configuration/Configuration.h"
#include <iostream>

using namespace std;

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

Vector3D StereoVision::convertStereoMeasureToVector(StereoSensorMeasure3D stereoMeasure)
{
	Vector3D v;

	v.x = stereoMeasure.x_mm;
	v.y = stereoMeasure.y_mm;
	v.z = stereoMeasure.z_mm;

	return v;
}

Vector3D StereoVision::convertStereoMeasureToVectorScaledToMeters(StereoSensorMeasure3D stereoMeasure)
{
	Vector3D v;

	v.x = stereoMeasure.x_mm * 0.001;
	v.y = stereoMeasure.y_mm * 0.001;
	v.z = stereoMeasure.z_mm * 0.001;

	return v;
}


vector<Point2f> StereoVision::project3DCoordinatesOnImagePlane(vector<Vector3D> coords_3d)
{
	ZEDStereoCameraHardwareParameters hwParam = Configuration::getInstance()->zedHardwareParameters;

	Mat objectPoints(1, coords_3d.size(), CV_32FC3);

	for (int i = 0; i < coords_3d.size(); i++) {
		objectPoints.at<Vec3f>(0, i) = Point3f(coords_3d[i].x, coords_3d[i].y, coords_3d[i].z);
		printf("StereoVision :: TRANSFERRING 3D COORD TO GPU DEVICE MEMORY: [%.2f, %.2f, %.2f]\n", coords_3d[i].x, coords_3d[i].y, coords_3d[i].z);
	}

	GpuMat d_objectPoints(objectPoints);
	GpuMat d_imagePoints;

	cout << "StereoVision :: Project 3D to Image Plane :: " << endl;
	cout << "\t===> Rotation Vector: " << hwParam.rotationVector_L << endl;
	cout << "\t===> Translation Vector: " << hwParam.translationVector_L << endl;
	cout << "\t===> Reference Camera Matrix: " << hwParam.cameraMatrix_L << endl;

	gpu::projectPoints(d_objectPoints, hwParam.rotationVector_L, hwParam.translationVector_L, hwParam.cameraMatrix_L, Mat(), d_imagePoints);

	Mat imagePoints(d_imagePoints);
	vector<Point2f> retval;
	for (int i = 0; i < coords_3d.size(); i++) {
		Vec2f image_coords = imagePoints.at<Vec2f>(0, i);
		Point2f p(image_coords.val[0], image_coords.val[1]);
		printf("StereoVision :: TRANSFERRING 3D COORD FROM GPU DEVICE TO HOST MEMORY: RES=[%.2f, %.2f]\n", image_coords.val[0], image_coords.val[1]);
		retval.push_back(p);
	}

	return retval;
}

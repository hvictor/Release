/*
 * ZEDStereoSensorDriver.h
 *
 *  Created on: Feb 1, 2016
 *      Author: sled
 */

#ifndef ZEDSTEREOSENSORDRIVER_H_
#define ZEDSTEREOSENSORDRIVER_H_

#include "StereoSensorAbstractionLayer.h"
#include <zed/Camera.hpp>
#include <zed/utils/GlobalDefine.hpp>
#include "../TargetPredator/TargetPredator.h"
#include "../StereoVision/IntrinsicParameters.h"
#include "../StereoVision/ExtrinsicParameters.h"

using namespace sl;

typedef struct
{
	int gain;
	int exposure;
} ZEDCameraProperties;

class ZEDStereoSensorDriver : public StereoSensorAbstractionLayer {
public:
	ZEDStereoSensorDriver();
	virtual ~ZEDStereoSensorDriver();

	bool openCamera();
	bool isOpen();
	bool closeCamera();
	StereoFrame fetchStereoFrame();
	ZEDCameraProperties *getZEDCameraProperties();
	void updateDepthFrameInterleave();

	static ZEDStereoSensorDriver *getInstance();
	static StereoSensorMeasure3D readMeasurementData3D(float *data, int x, int y, int step);
	static float readMeasurementDataConfidence(float *data, int x, int y, int step);
	static int retryTargetScan3D(pred_scan_t engage_data, float *xyz_data, int step_xyz, StereoSensorMeasure3D *measurement);
	static StereoSensorMeasure3D readMeasurementMatrix3D(Mat *xyzMat, int x, int y);
	static int retryTargetScanMatrix3D(pred_scan_t engage_data, Mat *xyzMat, StereoSensorMeasure3D *measurement);
	static float readMeasurementDataDepth(float *depth_data, int x, int y, int step);
	float repeatedDepthMeasure(int x, int y);
	StereoSensorMeasure3D triangulate(float z_depth, float x_image_plane, float y_image_plane);

private:
	ZEDCameraProperties *zedProperties;
	zed::Camera* zed;
	zed::MODE performanceMode;
	zed::SENSING_MODE sensingMode;

	bool computeDisparity;
	bool computeDepth;
	int frameCounter;
	int depthFrameInterleave;

	zed::CamParameters _param_L;
	zed::CamParameters _param_R;
};

#endif /* ZEDSTEREOSENSORDRIVER_H_ */

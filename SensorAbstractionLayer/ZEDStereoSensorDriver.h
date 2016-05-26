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

private:
	ZEDCameraProperties *zedProperties;
	zed::Camera* zed;
	zed::MODE performanceMode;
	zed::SENSING_MODE sensingMode;

	bool computeDisparity;
	bool computeDepth;
	int frameCounter;
	int depthFrameInterleave;
};

#endif /* ZEDSTEREOSENSORDRIVER_H_ */

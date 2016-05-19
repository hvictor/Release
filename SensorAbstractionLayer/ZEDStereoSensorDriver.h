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

using namespace sl;

typedef struct
{
	int gain;
	int exposure;
} ZEDCameraProperties;

typedef struct
{
	int x_mm;
	int y_mm;
	int z_mm;
} ZEDMeasure3D;

class ZEDStereoSensorDriver : public StereoSensorAbstractionLayer {
public:
	ZEDStereoSensorDriver();
	virtual ~ZEDStereoSensorDriver();

	bool openCamera();
	bool isOpen();
	bool closeCamera();
	StereoFrame fetchStereoFrame();
	ZEDCameraProperties *getZEDCameraProperties();
	ZEDMeasure3D readMeasure3D(int x, int y);
	void updateDepthFrameInterleave();
	static ZEDStereoSensorDriver *getInstance();

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

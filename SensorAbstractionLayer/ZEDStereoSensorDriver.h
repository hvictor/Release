/*
 * ZEDStereoSensorDriver.h
 *
 *  Created on: Feb 1, 2016
 *      Author: sled
 */

#ifndef ZEDSTEREOSENSORDRIVER_H_
#define ZEDSTEREOSENSORDRIVER_H_

#include "StereoSensorAbstractionLayer.h"

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
	bool closeCamera();
	StereoFrame fetchStereoFrame();
	ZEDCameraProperties *getZEDCameraProperties();

private:
	ZEDCameraProperties *zedProperties;
};

#endif /* ZEDSTEREOSENSORDRIVER_H_ */

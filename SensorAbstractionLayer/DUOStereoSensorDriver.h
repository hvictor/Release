/*
 * DUOStereoSensorDriver.h
 *
 *  Created on: Feb 1, 2016
 *      Author: sled
 */

#ifndef DUOSTEREOSENSORDRIVER_H_
#define DUOSTEREOSENSORDRIVER_H_

#include "StereoSensorAbstractionLayer.h"

typedef struct
{
	int gain;
	int exposure;
	int led;
} DUOCameraProperties;

class DUOStereoSensorDriver : public StereoSensorAbstractionLayer {
public:
	DUOStereoSensorDriver(DUOStereoCameraHardwareParameters duoHardwareParam);
	virtual ~DUOStereoSensorDriver();

	bool openCamera();
	bool closeCamera();
	StereoFrame fetchStereoFrame();
	DUOCameraProperties *getDUOCameraProperties();

private:
	DUOCameraProperties *duoProperties;
};

#endif /* DUOSTEREOSENSORDRIVER_H_ */

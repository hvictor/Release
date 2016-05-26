/*
 * StereoSensorAbstractionLayer.h
 *
 *  Created on: Feb 1, 2016
 *      Author: sled
 */

#ifndef STEREOSENSORABSTRACTIONLAYER_H_
#define STEREOSENSORABSTRACTIONLAYER_H_

#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include "../Configuration/Configuration.h"

typedef struct
{
	uint8_t *leftData;
	uint8_t *rightData;
	uint8_t *depthData;
	float 	*confidenceData;
	float	*xyzData;
	int	stepConfidence;
	int	stepXYZ;
	int stepDepth;
	int bytesLength;
	int channels;
	//Mat *xyzMat;
} StereoFrame;

typedef struct
{
	int width;
	int height;
} StereoFrameSize;

typedef struct
{
	float x_mm;
	float y_mm;
	float z_mm;
} StereoSensorMeasure3D;

class StereoSensorAbstractionLayer {
public:
	StereoSensorAbstractionLayer();
	virtual ~StereoSensorAbstractionLayer();
	virtual bool openCamera() = 0;
	virtual bool closeCamera() = 0;
	virtual bool isOpen() = 0;
	virtual StereoFrame fetchStereoFrame() = 0;
	StereoFrameSize getStereoFrameSize();

protected:
	StereoFrameSize frameSize;
	bool camera_open;
};

#endif /* STEREOSENSORABSTRACTIONLAYER_H_ */

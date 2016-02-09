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
	int bytesLength;
	int channels;
} StereoFrame;

typedef struct
{
	int width;
	int height;
} StereoFrameSize;

class StereoSensorAbstractionLayer {
public:
	StereoSensorAbstractionLayer();
	virtual ~StereoSensorAbstractionLayer();
	virtual bool openCamera() = 0;
	virtual bool closeCamera() = 0;
	virtual StereoFrame fetchStereoFrame() = 0;
	StereoFrameSize getStereoFrameSize();

protected:
	StereoFrameSize frameSize;
};

#endif /* STEREOSENSORABSTRACTIONLAYER_H_ */

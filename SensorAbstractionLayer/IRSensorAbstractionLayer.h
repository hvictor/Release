/*
 * IRSensorAbstractionLayer.h
 *
 *  Created on: Feb 1, 2016
 *      Author: sled
 */

#ifndef IRSENSORABSTRACTIONLAYER_H_
#define IRSENSORABSTRACTIONLAYER_H_

#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct
{
	uint8_t *depthData;
	int bytesLength;
} IrFrame;

typedef struct
{
	int width;
	int height;
} IrFrameSize;

class IRSensorAbstractionLayer {
public:
	IRSensorAbstractionLayer();
	virtual ~IRSensorAbstractionLayer();
	virtual bool openSensor() = 0;
	virtual bool closeSensor() = 0;
	virtual IrFrame fetchIrFrame() = 0;
	IrFrameSize getIrFrameSize();

protected:
	IrFrameSize frameSize;
};

#endif /* IRSENSORABSTRACTIONLAYER_H_ */

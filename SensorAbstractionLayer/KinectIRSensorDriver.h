/*
 * KinectIRSensorDriver.h
 *
 *  Created on: Feb 1, 2016
 *      Author: sled
 */

#ifndef KINECTIRSENSORDRIVER_H_
#define KINECTIRSENSORDRIVER_H_

#include "IRSensorAbstractionLayer.h"

class KinectIRSensorDriver : public IRSensorAbstractionLayer {
public:
	KinectIRSensorDriver();
	virtual ~KinectIRSensorDriver();

	// TODO: Not yet implemented
	virtual bool openSensor();
	virtual bool closeSensor();
	virtual IrFrame fetchIrFrame();
};

#endif /* KINECTIRSENSORDRIVER_H_ */

/*
 * PlayLogic.h
 *
 *  Created on: Jun 8, 2016
 *      Author: sled
 */

#ifndef PLAYLOGIC_H_
#define PLAYLOGIC_H_

#include "PlayScore.h"
#include "../Common/data_types.h"
#include "../Configuration/Configuration.h"
#include "../StaticModel/GroundModel.h"
#include "../StaticModel/NetModel.h"
#include "../StereoVision/StereoVision.h"
#include "../SensorAbstractionLayer/ZEDStereoSensorDriver.h"

using namespace std;
using namespace cv;

typedef struct
{
	Vector2D nearL, nearR;
	Vector2D farL, farR;
} PlayerFieldSide;

class PlayLogic {
public:
	PlayLogic();
	virtual ~PlayLogic();

	virtual void feedWithFloorBounceData(Vector3D floorBounceData, Vector2D opticalBounceData) = 0;
	virtual PlayScore *retrievePlayScore() = 0;
	virtual void generateFieldRepresentationFromModel() = 0;
};

#endif /* PLAYLOGIC_H_ */

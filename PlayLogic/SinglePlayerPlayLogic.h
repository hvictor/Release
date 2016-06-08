/*
 * SinglePlayerPlayLogic.h
 *
 *  Created on: Jun 8, 2016
 *      Author: sled
 */

#ifndef SINGLEPLAYERPLAYLOGIC_H_
#define SINGLEPLAYERPLAYLOGIC_H_

#include "PlayLogic.h"

class SinglePlayerPlayLogic : public PlayLogic
{
public:
	SinglePlayerPlayLogic();
	virtual ~SinglePlayerPlayLogic();

	void feedWithFloorBounceData(Vector3D floorBounceData);
	PlayScore *retrievePlayScore();
};

#endif /* SINGLEPLAYERPLAYLOGIC_H_ */

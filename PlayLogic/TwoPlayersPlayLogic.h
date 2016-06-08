/*
 * TwoPlayersPlayLogic.h
 *
 *  Created on: Jun 8, 2016
 *      Author: sled
 */

#ifndef TWOPLAYERSPLAYLOGIC_H_
#define TWOPLAYERSPLAYLOGIC_H_

#include "PlayLogic.h"
#include "TwoPlayersPlayScore.h"

class TwoPlayersPlayLogic : public PlayLogic {
public:
	TwoPlayersPlayLogic();
	virtual ~TwoPlayersPlayLogic();

	void feedWithFloorBounceData(Vector3D floorBounceData, Vector2D opticalBounceData);
	PlayScore *retrievePlayScore();

private:
	TwoPlayersPlayScore *_playScore;

};

#endif /* TWOPLAYERSPLAYLOGIC_H_ */

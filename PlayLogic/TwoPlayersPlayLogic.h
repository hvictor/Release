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

typedef struct
{
	PlayerFieldSide P1Field;
	PlayerFieldSide P2Field;
} TwoPlayersFieldRepresentation;

class TwoPlayersPlayLogic : public PlayLogic {
public:
	TwoPlayersPlayLogic();
	virtual ~TwoPlayersPlayLogic();

	void feedWithFloorBounceData(Vector3D floorBounceData, Vector2D opticalBounceData);
	void generateFieldRepresentationFromModel();
	PlayScore *retrievePlayScore();
	TwoPlayersFieldRepresentation retrieveFieldRepresentation();

private:
	TwoPlayersPlayScore *_playScore;
	PlayerFieldSide _P1Field;
	PlayerFieldSide _P2Field;

	NetVisualProjection _netProjection;
	NetCoordinates3D _netCoord3D;
};

#endif /* TWOPLAYERSPLAYLOGIC_H_ */

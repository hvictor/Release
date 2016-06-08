/*
 * TwoPlayersPlayLogic.cpp
 *
 *  Created on: Jun 8, 2016
 *      Author: sled
 */

#include "TwoPlayersPlayLogic.h"
#include "../StaticModel/TennisFieldStaticModel.h"

TwoPlayersPlayLogic::TwoPlayersPlayLogic()
{
	_playScore = new TwoPlayersPlayScore();
}

TwoPlayersPlayLogic::~TwoPlayersPlayLogic()
{
}

void TwoPlayersPlayLogic::feedWithFloorBounceData(Vector3D floorBounceData, Vector2D opticalBounceData)
{
	printf("Play Logic :: Two-Players :: Feed Data: 3D=[%.2f, %2f, %2f] 2D=[%.2f, %.2f]\n",
			floorBounceData.x, floorBounceData.y, floorBounceData.z,
			opticalBounceData.x, opticalBounceData.y);
}

PlayScore *TwoPlayersPlayLogic::retrievePlayScore()
{
	return _playScore;
}

void TwoPlayersPlayLogic::generateFieldRepresentationFromModel()
{
	// Fetch Net Model data
	_netProjection = NetModel::getInstance()->getNetVisualProjection();
	_netCoord3D = NetModel::getInstance()->getNetCoordinates3D();

	// Fetch 2D Field Model
	TennisFieldDelimiter *_tFieldDelimPtr = TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter();

	// Prepare the Players' field sides
	// If the Net Model is ready, use the Net as separator of the two sides
	if (NetModel::getInstance()->isReady()) {

		// Player1 Field
		_P1Field.nearL.x = (double)_tFieldDelimPtr->bottomLeft.x;
		_P1Field.nearL.y = (double)_tFieldDelimPtr->bottomLeft.y;

		_P1Field.nearR.x = (double)_netProjection.baseNear.x;
		_P1Field.nearR.y = (double)_netProjection.baseNear.y;

		_P1Field.farR.x = (double)_netProjection.baseFar.x;
		_P1Field.farR.y = (double)_netProjection.baseFar.y;

		_P1Field.farL.x = (double)_tFieldDelimPtr->topLeft.x;
		_P1Field.farL.y = (double)_tFieldDelimPtr->topLeft.y;

		// Player2 Field
		_P2Field.nearL.x = (double)_netProjection.baseNear.x;
		_P2Field.nearL.y = (double)_netProjection.baseNear.y;

		_P2Field.nearR.x = (double)_tFieldDelimPtr->bottomRight.x;
		_P2Field.nearR.y = (double)_tFieldDelimPtr->bottomRight.y;

		_P2Field.farR.x = (double)_tFieldDelimPtr->topRight.x;
		_P2Field.farR.y = (double)_tFieldDelimPtr->topRight.y;

		_P2Field.farL.x = (double)_netProjection.baseFar.x;
		_P2Field.farL.y = (double)_netProjection.baseFar.y;
	}
	// If the Net Model is not ready, separate the field in the middle
	else {

	}

}

TwoPlayersFieldRepresentation TwoPlayersPlayLogic::retrieveFieldRepresentation()
{
	TwoPlayersFieldRepresentation res;

	res.P1Field = _P1Field;
	res.P2Field = _P2Field;

	return res;
}


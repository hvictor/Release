/*
 * TwoPlayersPlayLogic.cpp
 *
 *  Created on: Jun 8, 2016
 *      Author: sled
 */

#include "TwoPlayersPlayLogic.h"

TwoPlayersPlayLogic::TwoPlayersPlayLogic()
{
	_playScore = new TwoPlayersPlayScore();
}

TwoPlayersPlayLogic::~TwoPlayersPlayLogic()
{
}

void TwoPlayersPlayLogic::feedWithFloorBounceData(Vector3D floorBounceData, Vector2D opticalBounceData)
{
	printf("Play Logic :: Two-Players :: Feed Data: 3D=[%.2f, %2f, %2f] 2D=[%d, %d]\n",
			floorBounceData.x, floorBounceData.y, floorBounceData.z,
			opticalBounceData.x, opticalBounceData.y);
}

PlayScore *TwoPlayersPlayLogic::retrievePlayScore()
{
	return _playScore;
}


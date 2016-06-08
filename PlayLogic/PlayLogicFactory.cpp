/*
 * PlayLogicFactory.cpp
 *
 *  Created on: Jun 8, 2016
 *      Author: sled
 */

#include "PlayLogicFactory.h"

PlayLogicFactory::PlayLogicFactory()
{
}

PlayLogicFactory::~PlayLogicFactory()
{
}

PlayLogicFactory *PlayLogicFactory::getInstance()
{
	static PlayLogicFactory *inst = 0;

	if (inst == 0)
	{
		inst = new PlayLogicFactory();
	}

	return inst;
}

TwoPlayersPlayLogic *PlayLogicFactory::requestTwoPlayersPlayLogic()
{
	return new TwoPlayersPlayLogic();
}

SinglePlayerPlayLogic *PlayLogicFactory::requestSinglePlayerPlayLogic()
{
	return new SinglePlayerPlayLogic();
}

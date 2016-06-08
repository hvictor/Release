/*
 * PlayLogicFactory.h
 *
 *  Created on: Jun 8, 2016
 *      Author: sled
 */

#ifndef PLAYLOGICFACTORY_H_
#define PLAYLOGICFACTORY_H_

#include "TwoPlayersPlayLogic.h"
#include "SinglePlayerPlayLogic.h"

class PlayLogicFactory {
public:
	static PlayLogicFactory *getInstance();
	virtual ~PlayLogicFactory();

	TwoPlayersPlayLogic 	*requestTwoPlayersPlayLogic();
	SinglePlayerPlayLogic	*requestSinglePlayerPlayLogic();
private:
	PlayLogicFactory();
};

#endif /* PLAYLOGICFACTORY_H_ */

/*
 * TwoPlayersPlayScore.h
 *
 *  Created on: Jun 8, 2016
 *      Author: sled
 */

#ifndef TWOPLAYERSPLAYSCORE_H_
#define TWOPLAYERSPLAYSCORE_H_

#include "PlayScore.h"

class TwoPlayersPlayScore : public PlayScore
{
public:
	TwoPlayersPlayScore();
	virtual ~TwoPlayersPlayScore();

	int Player1_Score;
	int Player1_Nets;
	int Player2_Score;
	int Player2_Nets;
};

#endif /* TWOPLAYERSPLAYSCORE_H_ */

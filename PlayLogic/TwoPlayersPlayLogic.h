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

enum ImpactResult
{
	Impact_Player1_Field,
	Impact_Player1_Line,
	Impact_Player2_Field,
	Impact_Player2_Line,
	Impact_CentralLine,
	Impact_Null,
};

enum LogicFSMState
{
	FSM_Idle,
	FSM_Wait_P1_Impact,
	FSM_Wait_P2_Impact,
	FSM_Missed_P1,
	FSM_Missed_P2
};

class TwoPlayersPlayLogic : public PlayLogic {
public:
	TwoPlayersPlayLogic();
	virtual ~TwoPlayersPlayLogic();

	void feedWithFloorBounceData(Vector3D floorBounceData, Vector2D opticalBounceData);
	void notifyTargetLost();
	void generateFieldRepresentationFromModel();
	PlayScore *retrievePlayScore();
	TwoPlayersFieldRepresentation retrieveFieldRepresentation();

private:
	void updateFSMState(Vector3D floorBounceData, Vector2D opticalBounceData);
	ImpactResult analyzeImpactEventData(double x, double y);
	ImpactResult __checkPlayer1Field(double x, double y);
	ImpactResult __checkPlayer2Field(double x, double y);

	// Finite State Machine state
	LogicFSMState _FSM_state;

	TwoPlayersPlayScore *_playScore;
	PlayerFieldSide _P1Field;
	PlayerFieldSide _P2Field;

	NetVisualProjection _netProjection;
	NetCoordinates3D _netCoord3D;
};

#endif /* TWOPLAYERSPLAYLOGIC_H_ */

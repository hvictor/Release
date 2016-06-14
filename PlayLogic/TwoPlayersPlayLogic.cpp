/*
 * TwoPlayersPlayLogic.cpp
 *
 *  Created on: Jun 8, 2016
 *      Author: sled
 */

#include "TwoPlayersPlayLogic.h"
#include "../StaticModel/TennisFieldStaticModel.h"
#include "../Common/converters.h"

TwoPlayersPlayLogic::TwoPlayersPlayLogic()
{
	_playScore = new TwoPlayersPlayScore();
	_FSM_state = FSM_Idle;
}

TwoPlayersPlayLogic::~TwoPlayersPlayLogic()
{
}

ImpactResult TwoPlayersPlayLogic::__checkPlayer1Field(double x, double y)
{
	vector<Point2f> vertices;

	vertices.push_back(vector2d_to_point2f(_P1Field.nearL));
	vertices.push_back(vector2d_to_point2f(_P1Field.nearR));
	vertices.push_back(vector2d_to_point2f(_P1Field.farR));
	vertices.push_back(vector2d_to_point2f(_P1Field.farL));

	double c = pointPolygonTest(vertices, Point2f((float)x, (float)y), true);

	double eps = (double)Configuration::getInstance()->getStaticModelParameters().linesSensitivityEPS;

	printf("Check P1 Field: c=%.2f, eps=%.2f\n", c, eps);

	if (c >= eps) {
		return Impact_Player1_Field;
	}
	else if (fabsl(c) < eps) {
		return Impact_Player1_Line;
	}

	return Impact_Null;
}

ImpactResult TwoPlayersPlayLogic::__checkPlayer2Field(double x, double y)
{
	vector<Point2f> vertices;

	vertices.push_back(vector2d_to_point2f(_P2Field.nearL));
	vertices.push_back(vector2d_to_point2f(_P2Field.nearR));
	vertices.push_back(vector2d_to_point2f(_P2Field.farR));
	vertices.push_back(vector2d_to_point2f(_P2Field.farL));

	double c = pointPolygonTest(vertices, Point2f((float)x, (float)y), true);

	double eps = (double)Configuration::getInstance()->getStaticModelParameters().linesSensitivityEPS;

	printf("Check P2 Field: c=%.2f, eps=%.2f\n", c, eps);

	// Inside Field
	if (c >= eps) {
		return Impact_Player2_Field;
	}

	// On the Line
	else if (fabsl(c) < eps) {
		return Impact_Player2_Line;
	}

	return Impact_Null;
}

ImpactResult TwoPlayersPlayLogic::analyzeImpactEventData(double x, double y)
{
	ImpactResult res_P1 = __checkPlayer1Field(x, y);
	printf("RES P1 = %d\n", res_P1);

	if (res_P1 == Impact_Player1_Field)
		return Impact_Player1_Field;

	ImpactResult res_P2 = __checkPlayer2Field(x, y);

	printf("RES P2 = %d\n--------\n", res_P2);

	if (res_P2 == Impact_Player2_Field)
		return Impact_Player2_Field;

	// Only happens without net
	if (res_P1 == Impact_Player1_Line && res_P2 == Impact_Player2_Line)
		return Impact_CentralLine;

	else if (res_P1 == Impact_Player1_Line && res_P2 == Impact_Null)
		return Impact_Player1_Line;

	else if (res_P2 == Impact_Player2_Line && res_P1 == Impact_Null)
		return Impact_Player2_Line;

	else
		return Impact_Null;
}

void TwoPlayersPlayLogic::notifyTargetLost()
{
	switch (_FSM_state)
	{
	case FSM_Wait_P1_Impact:
		//_FSM_state = FSM_Missed_P1; // Virtual transitory state
		// P1 Field was missed by P2: Assign point to P1
		printf("Two-Players PlayLogic :: NotifyTargetLost :: P1 Field was missed by P2: Assign point to P1\n");
		_playScore->Player1_Score++;

		_FSM_state = FSM_Idle;
		break;
	case FSM_Wait_P2_Impact:
		//_FSM_state = FSM_Missed_P2; // Virtual transitory state
		// P2 Field was missed by P1: Assign point to P2
		printf("Two-Players PlayLogic :: NotifyTargetLost :: P2 Field was missed by P1: Assign point to P2\n");
		_playScore->Player2_Score++;

		_FSM_state = FSM_Idle;
		printf("Two-Players PlayLogic :: NotifyTargetLost :: Idle, nothing to do\n");
		break;

	default:
		break;
	}
}

// Two-Players Logic Finite State Machine (FSM) Implementation
void TwoPlayersPlayLogic::updateFSMState(Vector3D floorBounceData, Vector2D opticalBounceData)
{
	// Analyze the current event
	ImpactResult res = analyzeImpactEventData(opticalBounceData.x, opticalBounceData.y);
	printf("Two-Players PlayLogic :: FSM :: Impact Event Data Analysis Result: res=%d\n", res);

	switch (_FSM_state)
	{
	case FSM_Idle:
		printf("FSM :: IDLE\n");
		// Not P1 or P2 impact: stay Idle
		// Impact in P1: start waiting for impact in P2
		if (res == Impact_Player1_Field)
			_FSM_state = FSM_Wait_P2_Impact;

		// Impact in P2: start waiting for impact in P1
		else if (res == Impact_Player2_Field)
			_FSM_state = FSM_Wait_P1_Impact;

		break;

	case FSM_Wait_P1_Impact:
		printf("FSM :: WAITING P1 IMPACT\n");
		// Impact in P1: start waiting for imapct in P2
		if (res == Impact_Player1_Field)
			_FSM_state = FSM_Wait_P2_Impact;
		else {
			//_FSM_state = FSM_Missed_P1; // Virtual state, transitory

		}

		// Reset Automata
		_FSM_state = FSM_Idle;

		break;

	case FSM_Wait_P2_Impact:
		printf("FSM :: WAITING P2 IMPACT\n");
		// Impact in P2: start waiting for imapct in P1
		if (res == Impact_Player2_Field)
			_FSM_state = FSM_Wait_P1_Impact;
		else {
			//_FSM_state = FSM_Missed_P2; // Virtual state, transitory


		}

		// Reset Automata
		_FSM_state = FSM_Idle;

		break;

	default:
		printf("FSM: Unknown status: %d\n", _FSM_state);
		break;
	}
}

void TwoPlayersPlayLogic::feedWithFloorBounceData(Vector3D floorBounceData, Vector2D opticalBounceData)
{
	printf("Play Logic :: Two-Players :: Feed Data: 3D=[%.2f, %2f, %2f] 2D=[%.2f, %.2f]\n",
			floorBounceData.x, floorBounceData.y, floorBounceData.z,
			opticalBounceData.x, opticalBounceData.y);

	updateFSMState(floorBounceData, opticalBounceData);
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


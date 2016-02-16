/*
 * TrajectoryReconstructor.cpp
 *
 *  Created on: Jan 29, 2016
 *      Author: sled
 */

#include "TrajectoryReconstructor.h"

TrajectoryReconstructor::TrajectoryReconstructor()
{
	interpEngine = InterpolationEngine::getInstance();
}

TrajectoryReconstructor::~TrajectoryReconstructor()
{
	delete interpEngine;
}

CubicPolynomialCoeff TrajectoryReconstructor::convertToCubicPolynomial(StateRelatedTable *stateTable, int indexFrom, int indexTo)
{
	CubicPolynomialCoeff c;

	return interpEngine->interpolateCubicPolynomial(stateTable->relatedStates, indexFrom, indexTo);
}

HexaPolynomialCoeff TrajectoryReconstructor::convertToHexaPolynomial(StateRelatedTable *stateTable, int indexFrom, int indexTo)
{
	HexaPolynomialCoeff c;

	return interpEngine->interpolateHexaPolynomial(stateTable->relatedStates, indexFrom, indexTo);
}

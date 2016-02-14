/*
 * TrajectoryReconstructor.cpp
 *
 *  Created on: Jan 29, 2016
 *      Author: sled
 */

#include "TrajectoryReconstructor.h"

TrajectoryReconstructor::TrajectoryReconstructor()
{
	interpEngine = new InterpolationEngine();
}

TrajectoryReconstructor::~TrajectoryReconstructor()
{
	delete interpEngine;
}

CubicPolynomialCoeff TrajectoryReconstructor::convertToCubicPolynomial(StateRelatedTable *stateTable)
{
	CubicPolynomialCoeff c;

	return interpEngine->interpolateCubicPolynomial(stateTable->relatedStates);
}


/*
 * TrajectoryReconstructor.h
 *
 *  Created on: Jan 29, 2016
 *      Author: sled
 */

#ifndef TRAJECTORYRECONSTRUCTOR_H_
#define TRAJECTORYRECONSTRUCTOR_H_

#include "../InterpolationEngine/InterpolationEngine.h"
#include "../Common/opencv_headers.h"


class TrajectoryReconstructor {
public:
	TrajectoryReconstructor();
	virtual ~TrajectoryReconstructor();
	CubicPolynomialCoeff convertToCubicPolynomial(StateRelatedTable *stateTable, int indexFrom, int indexTo);
	HexaPolynomialCoeff convertToHexaPolynomial(StateRelatedTable *stateTable, int indexFrom, int indexTo);

private:
	InterpolationEngine *interpEngine;
};

#endif /* TRAJECTORYRECONSTRUCTOR_H_ */

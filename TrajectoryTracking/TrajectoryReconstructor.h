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
	CubicPolynomialCoeff convertToCubicPolynomial(StateRelatedTable *stateTable);
	HexaPolynomialCoeff convertToHexaPolynomial(StateRelatedTable *stateTable);

private:
	InterpolationEngine *interpEngine;
};

#endif /* TRAJECTORYRECONSTRUCTOR_H_ */

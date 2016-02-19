/*
 * SlopeBehaviourAnalyzer.h
 *
 *  Created on: Feb 14, 2016
 *      Author: sled
 */

#ifndef SLOPEBEHAVIOURANALYZER_H_
#define SLOPEBEHAVIOURANALYZER_H_

#include "../Configuration/Configuration.h"
#include "../Common/opencv_headers.h"
#include "../OpticalLayer/StatefulObjectFilter.h"
#include "InterpolationEngine.h"

#define	EXTREMA_TYPE_LOCAL_MIN		0
#define EXTREMA_TYPE_LOCAL_MAX		1

class SlopeBehaviourAnalyzer {
public:
	static SlopeBehaviourAnalyzer *getInstance();
	virtual ~SlopeBehaviourAnalyzer();
	vector<int> computeLocalMinima(HexaPolynomialCoeff coeffs, vector<TrackedState *> trackedStates, int indexFrom, int indexTo);
	int searchNearestLocalMinimumDiscrete(HexaPolynomialCoeff coeffs, vector<TrackedState *> trackedStates, int indexFrom, int indexTo);
	int extremaType(HexaPolynomialCoeff coeffs, double x, double x_from, double x_to);
private:
	SlopeBehaviourAnalyzer();
};

#endif /* SLOPEBEHAVIOURANALYZER_H_ */

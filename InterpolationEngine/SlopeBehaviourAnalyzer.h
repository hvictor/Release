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

class SlopeBehaviourAnalyzer {
public:
	static SlopeBehaviourAnalyzer *getInstance();
	virtual ~SlopeBehaviourAnalyzer();
	vector<int> computeLocalMinima(HexaPolynomialCoeff coeffs, vector<TrackedState *> trackedStates, int indexFrom, int indexTo);
	int searchNearestLocalMinimumDiscrete(HexaPolynomialCoeff coeffs, vector<TrackedState *> trackedStates, int indexFrom, int indexTo);
private:
	SlopeBehaviourAnalyzer();
};

#endif /* SLOPEBEHAVIOURANALYZER_H_ */

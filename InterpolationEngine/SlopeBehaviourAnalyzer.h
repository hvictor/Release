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

class SlopeBehaviourAnalyzer {
public:
	static SlopeBehaviourAnalyzer *getInstance();
	SlopeBehaviourAnalyzer();
	virtual ~SlopeBehaviourAnalyzer();
	vector<int> quadraticGetQuickSlopeVariationIndexes(vector<TrackedState *> trackedStates);

private:

};

#endif /* SLOPEBEHAVIOURANALYZER_H_ */

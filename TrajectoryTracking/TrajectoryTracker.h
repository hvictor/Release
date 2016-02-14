/*
 * TrajectoryTracker.h
 *
 *  Created on: Feb 14, 2016
 *      Author: sled
 */

#ifndef TRAJECTORYTRACKER_H_
#define TRAJECTORYTRACKER_H_

#include "../Configuration/Configuration.h"
#include "../Common/opencv_headers.h"
#include "../OpticalLayer/StatefulObjectFilter.h"
#include "TrajectoryDescriptor.h"
#include "TrajectoryReconstructor.h"

class TrajectoryTracker {
public:
	TrajectoryTracker();
	virtual ~TrajectoryTracker();
	void update(StateRelatedTable *trajectoryCandidate);
	vector<TrajectoryDescriptor *> getCurrentTrackingState();

private:
	vector<TrajectoryDescriptor *> trackingTable;
	TrajectoryReconstructor *reconstructor;
	TrajectoryDescriptor *searchTrajectoryDescriptor(int ID);
};

#endif /* TRAJECTORYTRACKER_H_ */

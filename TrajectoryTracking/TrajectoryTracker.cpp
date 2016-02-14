/*
 * TrajectoryTracker.cpp
 *
 *  Created on: Feb 14, 2016
 *      Author: sled
 */

#include "TrajectoryTracker.h"

TrajectoryTracker::TrajectoryTracker()
{
	reconstructor = new TrajectoryReconstructor();
}

TrajectoryTracker::~TrajectoryTracker()
{
	delete reconstructor;
}

TrajectoryDescriptor *TrajectoryTracker::searchTrajectoryDescriptor(int ID)
{
	for (int i = 0; i < trackingTable.size(); i++) {
		if (trackingTable[i]->getID() == ID) {
			return trackingTable[i];
		}
	}

	return NULL;
}

void TrajectoryTracker::update(StateRelatedTable *trajectoryCandidate)
{
	TrajectoryDescriptor *trajectoryDescriptor = searchTrajectoryDescriptor(trajectoryCandidate->stateTableID);

	// Create new Trajectory Descriptor
	if (trajectoryDescriptor == NULL) {
		trajectoryDescriptor = new TrajectoryDescriptor(trajectoryCandidate->stateTableID);
		trackingTable.push_back(trajectoryDescriptor);
	}

	// Update the descriptor with the actual polynomial approximation
	CubicPolynomialCoeff coeff = reconstructor->convertToCubicPolynomial(trajectoryCandidate);
	double xFrom = std::min((double)trajectoryCandidate->relatedStates[0]->state.x, (double)trajectoryCandidate->relatedStates[trajectoryCandidate->relatedStates.size() - 1]->state.x);
	double xTo = std::max((double)trajectoryCandidate->relatedStates[0]->state.x, (double)trajectoryCandidate->relatedStates[trajectoryCandidate->relatedStates.size() - 1]->state.x);

	trajectoryDescriptor->update(coeff, xFrom, xTo);
}

vector<TrajectoryDescriptor *> TrajectoryTracker::getCurrentTrackingState()
{
	return trackingTable;
}

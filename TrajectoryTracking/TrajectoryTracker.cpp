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
	printf("searchTrajectoryDescriptor: tracking table size = %d\n", trackingTable.size());
	for (int i = 0; i < trackingTable.size(); i++) {
		if (trackingTable[i]->getID() == ID) {
			return trackingTable[i];
		}
	}

	return NULL;
}

void TrajectoryTracker::update(StateRelatedTable *trajectoryCandidate)
{
	if (trajectoryCandidate == NULL) {
		return;
	}

	TrajectoryDescriptor *trajectoryDescriptor = searchTrajectoryDescriptor(trajectoryCandidate->stateTableID);

	// Create new Trajectory Descriptor
	if (trajectoryDescriptor == NULL) {
		printf("TrajectoryTracker :: Update :: Not found, creating new one\n");
		trajectoryDescriptor = new TrajectoryDescriptor(trajectoryCandidate->stateTableID);
		trackingTable.push_back(trajectoryDescriptor);
	}

	// Update the descriptor with the actual polynomial approximation
	printf("TrajectoryTracker :: Update :: Converting to polynomial...\n");
	CubicPolynomialCoeff coeff = reconstructor->convertToCubicPolynomial(trajectoryCandidate);
	double xFrom = std::min((double)trajectoryCandidate->relatedStates[0]->state.x, (double)trajectoryCandidate->relatedStates[trajectoryCandidate->relatedStates.size() - 1]->state.x);
	double xTo = std::max((double)trajectoryCandidate->relatedStates[0]->state.x, (double)trajectoryCandidate->relatedStates[trajectoryCandidate->relatedStates.size() - 1]->state.x);

	printf("TrajectoryTracker :: Update :: Converted, got coefficients %g %g %g %g\n", coeff.c0, coeff.c1, coeff.c2, coeff.c3);

	trajectoryDescriptor->update(coeff, xFrom, xTo);

	printf("TrajectoryTracker :: Update :: Update completed\n");
}

vector<TrajectoryDescriptor *> TrajectoryTracker::getCurrentTrackingState()
{
	return trackingTable;
}

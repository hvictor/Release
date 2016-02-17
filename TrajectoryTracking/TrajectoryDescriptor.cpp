/*
 * TrajectoryDescriptor.cpp
 *
 *  Created on: Feb 14, 2016
 *      Author: sled
 */

#include "TrajectoryDescriptor.h"
#include "../InterpolationEngine/InterpolationEngine.h"
#include "../InterpolationEngine/SlopeBehaviourAnalyzer.h"

TrajectoryDescriptor::TrajectoryDescriptor(int ID)
{
	this->ID = ID;
}

TrajectoryDescriptor::~TrajectoryDescriptor()
{
}

int TrajectoryDescriptor::getID()
{
	return ID;
}

vector<TrajectorySection *> TrajectoryDescriptor::getTrajectorySections()
{
	return trajectorySections;
}

void TrajectoryDescriptor::update(CubicPolynomialCoeff coeffs, double xFrom, double xTo, vector<TrackedState *> trackedStates, bool reverse)
{
	// Descriptor is empty
	if (!trajectorySections.size()) {
		TrajectorySection *s = new TrajectorySection();

		s->coeffs = coeffs;
		s->x_from = xFrom;
		s->x_to = xTo;
		s->optimized = false;

		trajectorySections.push_back(s);
	}

	// Update only last section
	TrajectorySection *lastSection = trajectorySections[trajectorySections.size() - 1];
	lastSection->coeffs = coeffs;
	lastSection->x_from = xFrom;
	lastSection->x_to = xTo;

	// Optimize: the last trajectory section could be splitted into two more precisely
	// approximating polynomials
	optimize(trackedStates);
}

void TrajectoryDescriptor::update(HexaPolynomialCoeff coeffs, double xFrom, double xTo, int indexFrom, int indexTo, vector<TrackedState *> trackedStates, bool reverse)
{
	printf("[X FROM] %g\n", xFrom);

	// Descriptor is empty
	if (!trajectorySections.size()) {
		TrajectorySection *s = new TrajectorySection();

		s->hexa_coeffs = coeffs;
		s->x_from = xFrom;
		s->x_to = xTo;
		s->index_from = indexFrom;
		s->index_to = indexTo;
		s->optimized = false;

		trajectorySections.push_back(s);

		return;
	}

	// Update only last section
	TrajectorySection *lastSection = trajectorySections[trajectorySections.size() - 1];
	lastSection->hexa_coeffs = coeffs;

	lastSection->x_to = xTo;
	lastSection->index_to = indexTo;

	printf("TrajectoryDescriptor[%d] Updated: From %g to %g\n", ID, lastSection->x_from, lastSection->x_to);

	// Optimize: the last trajectory section could be splitted into two more precisely
	// approximating polynomials
	optimize(trackedStates);
}

void TrajectoryDescriptor::optimize(vector<TrackedState *> trackedStates)
{
	// Get last trajectory section
	TrajectorySection *lastSection = trajectorySections[trajectorySections.size() - 1];
	vector<int> localMinimaIndexes = SlopeBehaviourAnalyzer::getInstance()->computeLocalMinima(lastSection->hexa_coeffs, trackedStates, lastSection->index_from, lastSection->index_to);

	if (!localMinimaIndexes.size()) {
		return;
	}

	for (int i = 0; i < localMinimaIndexes.size(); i++) {

		// Set the current local minimum as the trajectory section's last point
		lastSection->index_to = localMinimaIndexes[i];
		lastSection->x_to = (double)trackedStates[localMinimaIndexes[i]]->state.x;

		// Create the next trajectory section consequent to splitting the last in two parts:
		// - The optimized part, with a polynomial of order 3
		// - The new part, with the original polynomial of order 6
		TrajectorySection *newSection = new TrajectorySection();
		newSection->hexa_coeffs = lastSection->hexa_coeffs;
		newSection->index_from = lastSection->index_to;
		newSection->index_to = newSection->index_from; // Initialization
		newSection->x_to = newSection->x_from; // Initialization
		newSection->optimized = false;

		// Approximate the last trajectory section with a cubic polynomial
		approxTrajectorySectionCubic(lastSection, trackedStates);

		// Add the new section
		trajectorySections.push_back(newSection);

		// Update last section pointer
		lastSection = trajectorySections[trajectorySections.size() - 1];
	}


}

void TrajectoryDescriptor::approxTrajectorySectionCubic(TrajectorySection *s, vector<TrackedState *> trackedStates)
{
	s->coeffs = InterpolationEngine::getInstance()->interpolateCubicPolynomial(trackedStates, s->index_from, s->index_to);
	s->optimized = true;
}

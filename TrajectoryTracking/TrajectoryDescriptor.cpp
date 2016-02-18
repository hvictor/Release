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

	// Optimize: the last trajectory section could be splitted into two more precisely
	// approximating polynomials

	// Do not optimize if the last section is described by less than 4 states
	if (lastSection->index_to - lastSection->index_from + 1 < 4) {
		return;
	}

	optimize(trackedStates);
}

void TrajectoryDescriptor::optimize(vector<TrackedState *> trackedStates)
{
	// Get last trajectory section
	TrajectorySection *lastSection = trajectorySections[trajectorySections.size() - 1];
	vector<int> localMinimaIndexes = SlopeBehaviourAnalyzer::getInstance()->computeLocalMinima(lastSection->hexa_coeffs, trackedStates, lastSection->index_from, lastSection->index_to);

	if (!localMinimaIndexes.size() || (lastSection->index_to - lastSection->index_from + 1) < 4) {
		return;
	}

	for (int i = 0; i < localMinimaIndexes.size(); i++) {

		// Set the current local minimum as the trajectory section's last point
		lastSection->index_to = localMinimaIndexes[i];
		lastSection->x_to = (double)trackedStates[localMinimaIndexes[i]]->state.x;

		HexaPolynomialCoeff lastHexaPolyCoeff = lastSection->hexa_coeffs;
		int lastIndexFrom = lastSection->index_from;
		int lastIndexTo = lastSection->index_to;
		double lastXFrom = lastSection->x_from;
		double lastXTo = lastSection->x_to;

		// If the last section is too short, aggregate it to the previous if any
		if (lastSection->index_to - lastSection->index_from + 1 < 4) {

			if (trajectorySections.size() > 1) {
				// Extend the previous-last section to cover the last section's states
				TrajectorySection *prevLastSection = trajectorySections[trajectorySections.size() - 2];
				prevLastSection->index_to = lastSection->index_to;

				// Re-interpolate the approximating cubic polynomial for the previous-last section, now covering the last section states
				prevLastSection->coeffs = InterpolationEngine::getInstance()->interpolateCubicPolynomial(trackedStates, prevLastSection->index_from, prevLastSection->index_to);

			}

			// Delete last section
			trajectorySections.erase(trajectorySections.end() - 1);

			// Update last section pointer
			if (trajectorySections.size() > 0) {
				lastSection = trajectorySections[trajectorySections.size() - 1]; // That is prevLastSection
			}
			else {
				lastSection = 0;
			}
		}

		// Create the next trajectory section consequent to splitting the last in two parts:
		// - The optimized part, with a polynomial of order 3
		// - The new part, with the original polynomial of order 6
		TrajectorySection *newSection = new TrajectorySection();

		// If the last section was the only one, and it was removed because too short,
		// the new section replaces the just removed last section
		if (!lastSection) {
			newSection->hexa_coeffs = lastHexaPolyCoeff;
			newSection->index_from = lastIndexFrom;
			newSection->index_to = lastIndexTo;
			newSection->x_from = lastXFrom;
			newSection->x_to = lastXTo;

			trajectorySections.push_back(newSection);
		}
		else {
			newSection->hexa_coeffs = lastSection->hexa_coeffs;
			newSection->index_from = lastSection->index_to;
			newSection->index_to = newSection->index_from; // Initialization

			newSection->x_from = lastSection->x_to;
			newSection->x_to = newSection->x_from; // Initialization
			newSection->optimized = false;

			// Approximate the last trajectory section with a cubic polynomial
			approxTrajectorySectionCubic(lastSection, trackedStates);

			// Add the new section
			trajectorySections.push_back(newSection);
		}

		// Update last section pointer
		lastSection = trajectorySections[trajectorySections.size() - 1];
	}
}

void TrajectoryDescriptor::approxTrajectorySectionCubic(TrajectorySection *s, vector<TrackedState *> trackedStates)
{
	s->coeffs = InterpolationEngine::getInstance()->interpolateCubicPolynomial(trackedStates, s->index_from, s->index_to);
	s->optimized = true;
}

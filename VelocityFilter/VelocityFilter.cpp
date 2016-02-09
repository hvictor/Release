/*
 * VelocityFilter.cpp
 *
 *  Created on: Jan 29, 2016
 *      Author: sled
 */

#include "VelocityFilter.h"

VelocityFilter::VelocityFilter()
{
}

VelocityFilter::~VelocityFilter()
{
}

void VelocityFilter::correctVelocityVectors(StateRelatedTable *statesTable)
{
	// Iterate the states observing 3 states at a time.
	// The velocity vector measurement is an outlier if its steepness is not monotonic between the 3 states.

	// No information enough to correct something
	if (statesTable->relatedStates.size() < 3) {
		return;
	}

	vector<TrackedState *> s = statesTable->relatedStates;

	for (int c = 0; c < s.size() - 2; c++) {
		TrackedState *i = s[c];
		TrackedState *j = s[c + 1];
		TrackedState *k = s[c + 2];

		// Velocity vectors scaled by a factor 10.0 to avoid numeric errors
		double vx_i = (i->state.x - i->state.displacement_x) * 10.0;
		double vy_i = (i->state.y - i->state.displacement_y) * 10.0;

		double vx_j = (j->state.x - j->state.displacement_x) * 10.0;
		double vy_j = (j->state.y - j->state.displacement_y) * 10.0;

		double vx_k = (k->state.x - k->state.displacement_x) * 10.0;
		double vy_k = (k->state.y - k->state.displacement_y) * 10.0;

		// Steepness (steepness = y / x)
		double steep_i = vy_i / vx_i;
		double steep_j = vy_j / vx_j;
		double steep_k = vy_k / vx_k;

		// If steepness is not monotonic, the j velocity vector is an outlier
		if (!(steep_i <= steep_j && steep_j <= steep_k) || (steep_i >= steep_j && steep_j >= steep_k)) {
			// Recompute j
			double deltax = (k->state.x - i->state.x) / 2.0;
			double deltay = (k->state.y - i->state.y) / 2.0;

			j->state.x = i->state.x + deltax;
			j->state.y = i->state.y + deltay;
			j->state.displacement_x = deltax;
			j->state.displacement_y = deltay;
		}
	}
}

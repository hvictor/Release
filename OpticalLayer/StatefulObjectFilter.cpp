/*
 * StatefulObjectFilter.cpp
 *
 *  Created on: Jan 28, 2016
 *      Author: sled
 */

#include "StatefulObjectFilter.h"
#include <math.h>
#include "../VelocityFilter/VelocityFilter.h"
#include "../Configuration/Configuration.h"

StatefulObjectFilter::StatefulObjectFilter() {
	timer = 0;
}

StatefulObjectFilter::~StatefulObjectFilter() {

}

StateRelatedTable *StatefulObjectFilter::updateFilterState(FlowObject flowObject)
{
	TrackedState *state = makeTrackedState(flowObject);
	return relateStateToTable(state);
}

void StatefulObjectFilter::tick()
{
	timer++;
}

void StatefulObjectFilter::deleteTable(StateRelatedTable *table)
{
	for (int i = 0; i < table->relatedStates.size(); i++) {
		delete table->relatedStates[i];
	}

	delete table;
}

bool StatefulObjectFilter::suppressionNeeded(StateRelatedTable *t)
{
	return ((timer - t->updateTick) >= Configuration::getInstance()->getOpticalLayerParameters().statefulObjectFilterIdleMaxTicks);
}

vector<StateRelatedTable *> StatefulObjectFilter::getTables()
{
	return tables;
}

vector<StateRelatedTable *> StatefulObjectFilter::getTrajectoryCandidateTables()
{
	vector<StateRelatedTable *> c;

	for (vector<StateRelatedTable *>::iterator s = tables.begin(); s != tables.end(); ) {

		if (suppressionNeeded(*s)) {
			deleteTable(*s);
			s = tables.erase(s);
			continue;
		}
		else if ((*s)->relatedStates.size() >= Configuration::getInstance()->getOpticalLayerParameters().statefulObjectFilterTrajectoryMinStates) {
			c.push_back(*s);
		}

		if (s != tables.end()) s++;
	}

	return c;
}

int StatefulObjectFilter::getTablesCount()
{
	return tables.size();
}

bool StatefulObjectFilter::relatedDisplacement(TrackedState *state, StateRelatedTable *table)
{
	// Apply the velocity filter to correct outliers
	VelocityFilter::correctVelocityVectors(table);

	// Get table's latest state
	TrackedState *s = table->relatedStates[table->relatedStates.size() - 1];

	// The current state is related with this table, iff its position corresponds
	// to the latest state's position plus the displacement
	double diff_x = fabsl((double)state->state.x - (double)((double)s->state.x + s->state.displacement_x));
	double diff_y = fabsl((double)state->state.y - (double)((double)s->state.y + s->state.displacement_y));

	return (diff_x <= Configuration::getInstance()->getOpticalLayerParameters().statefulObjectFilterRelatedMaxDiffDirectionalX && diff_y <= Configuration::getInstance()->getOpticalLayerParameters().statefulObjectFilterRelatedMaxDiffDirectionalY);
}

bool StatefulObjectFilter::relatedPrecise(TrackedState *state, StateRelatedTable *table)
{
	bool retval = false;
	TrackedState *s = table->relatedStates[table->relatedStates.size() - 1];
/*
	for (int i = table->currentUpdateFirstIndex; i < table->relatedStates.size(); i++) {
		TrackedState *s = table->relatedStates[i];
		if (s->state.nextPosition == state->state.currentPosition) {
			retval = true;
			break;
		}
	}
*/
	return (s->state.nextPosition == state->state.currentPosition);
	//return retval;
}

bool StatefulObjectFilter::relatedPreciseMultipleStates(TrackedState *state, StateRelatedTable *table)
{
	bool retval = false;
	int t = table->relatedStates[table->relatedStates.size() - 1]->t;
	double d = Configuration::getInstance()->getOpticalLayerParameters().statefulObjectFilterRelatedMaxDiffDirectionalX;

	for (int i = table->relatedStates.size() - 1; i >= 0; i--) {
		if (table->relatedStates[i]->t != t) {
			break;
		}

		TrackedState *s = table->relatedStates[i];

		Point p = s->state.nextPosition;
		Point q = state->state.currentPosition;
		double dx = fabsl(q.x - p.x);
		double dy = fabsl(q.y - p.y);
		if (sqrt(dx*dx + dy*dy) < d) {
			retval = true;
			break;
		}
		/*
		if (s->state.nextPosition == state->state.currentPosition) {
			retval = true;
			break;
		}
		*/
	}

	return retval;
}

bool StatefulObjectFilter::related(TrackedState *state, StateRelatedTable *table)
{
	// Get table's latest state
	TrackedState *s = table->relatedStates[0];

	// The current state is related with this table, iff its position corresponds
	// to the latest state's position plus the displacement
	int argmax = 0;
	double maxDist;
	double diff_x = fabsl((double)state->state.x - (double)((double)s->state.x));
	double diff_y = fabsl((double)state->state.y - (double)((double)s->state.y));

	maxDist = sqrt(diff_x*diff_x + diff_y*diff_y);

	for (int j = 1; j < table->relatedStates.size(); j++) {
		s = table->relatedStates[j];
		diff_x = fabsl((double)state->state.x - (double)((double)s->state.x));
		diff_y = fabsl((double)state->state.y - (double)((double)s->state.y));

		if (sqrt(diff_x*diff_x + diff_y*diff_y) <= maxDist) {
			maxDist = sqrt(diff_x*diff_x + diff_y*diff_y);
			argmax = j;
		}
	}

	diff_x = fabsl((double)state->state.x - (double)((double)table->relatedStates[argmax]->state.x));
	diff_y = fabsl((double)state->state.y - (double)((double)table->relatedStates[argmax]->state.y));

	return (sqrt(diff_x*diff_x + diff_y*diff_y) <= Configuration::getInstance()->getOpticalLayerParameters().statefulObjectFilterRelatedMaxDiffSphereX);
}

StateRelatedTable *StatefulObjectFilter::relateStateToTable(TrackedState *state)
{
	StateRelatedTable *stateTable = NULL;

	for (int i = 0; i < tables.size(); i++) {
		if (relatedPreciseMultipleStates(state, tables[i])) {
			stateTable = tables[i];
			break;
		}
	}

	if (stateTable == NULL) {
		stateTable = new StateRelatedTable();
		stateTable->stateTemporalLayers = 0;
		stateTable->stateTableID = tables.size();
		tables.push_back(stateTable);
	}

	if (stateTable->currentUpdateFirstIndex < 0) {
		stateTable->currentUpdateFirstIndex = stateTable->relatedStates.size();
	}

	stateTable->updateTick = timer;
	stateTable->relatedStates.push_back(state);

	return stateTable;
}

void StatefulObjectFilter::computeMeanMotionCenters()
{
	for (int i = 0; i < tables.size(); i++) {
		StateRelatedTable *t = tables[i];

		if (t->currentUpdateFirstIndex < 0 || t->relatedStates.size() < 2)
			continue;

		double accum_displacementx = 0.0;
		double accum_displacementy = 0.0;

		double xaccum = 0.0;
		double yaccum = 0.0;

		double next_xaccum = 0.0;
		double next_yaccum = 0.0;

		double xmin = RAND_MAX;
		double xmax = -RAND_MAX;
		double ymin = RAND_MAX;
		double ymax = -RAND_MAX;
		double tot = (double)t->relatedStates.size() - (double)t->currentUpdateFirstIndex + 1.0;

		for (vector<TrackedState *>::iterator is = (t->relatedStates.begin() + t->currentUpdateFirstIndex); is != t->relatedStates.end(); ) {

			// TODO: NO XMIN E XMAX, MEDIA X, Y
			// TODO: VETTORI: MEDIA --> Vettori con (differenza risp. media) > dev std ---> scartare
			if ((double)(*is)->state.x < xmin) xmin = (double)(*is)->state.x;
			if ((double)(*is)->state.y < ymin) ymin = (double)(*is)->state.y;
			if ((double)(*is)->state.x > xmax) xmax = (double)(*is)->state.x;
			if ((double)(*is)->state.y > ymax) ymax = (double)(*is)->state.y;

			xaccum += (double)(*is)->state.x;
			yaccum += (double)(*is)->state.y;

			next_xaccum += (double)(*is)->state.nextPosition.x;
			next_yaccum += (double)(*is)->state.nextPosition.y;

			accum_displacementx += (*is)->state.displacement_x;
			accum_displacementy += (*is)->state.displacement_y;

			is = t->relatedStates.erase(is);
		}

		accum_displacementx /= tot;
		accum_displacementy /= tot;

		xaccum /= tot;
		yaccum /= tot;

		next_xaccum /= tot;
		next_yaccum /= tot;

		FlowObject tmp;
		tmp.displacement_x = accum_displacementx;
		tmp.displacement_y = accum_displacementy;
		tmp.x = xaccum;
		tmp.y = yaccum;
		tmp.currentPosition.x = tmp.x;
		tmp.currentPosition.y = tmp.x;
		tmp.nextPosition.x = next_xaccum;
		tmp.nextPosition.y = next_yaccum;

		t->relatedStates.push_back(makeTrackedState(tmp));

		t->currentUpdateFirstIndex = -1;
	}
}

TrackedState *StatefulObjectFilter::makeTrackedState(FlowObject flowObject)
{
	TrackedState *trackedState = new TrackedState();

	trackedState->t = timer;
	trackedState->state = flowObject;

	return trackedState;
}

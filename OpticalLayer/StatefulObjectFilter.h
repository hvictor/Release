/*
 * StatefulObjectFilter.h
 *
 *  Created on: Jan 28, 2016
 *      Author: sled
 */

#ifndef STATEFULOBJECTFILTER_H_
#define STATEFULOBJECTFILTER_H_

#include "FlowProcessor.h"

typedef struct state_related_table StateRelatedTable;

typedef struct
{
	FlowObject state;
	uint64_t t;
} TrackedState;

struct state_related_table
{
	int stateTableID;
	int updateTick;
	int temporalLayers;
	vector<TrackedState *> relatedStates;

	int currentUpdateFirstIndex;

	bool operator==(const StateRelatedTable& other) const
	{
		return (stateTableID < other.stateTableID);
	}

	bool operator<(const StateRelatedTable& other) const
	{
		return (stateTableID == other.stateTableID);
	}
};

class StatefulObjectFilter {
public:
	StatefulObjectFilter();
	virtual ~StatefulObjectFilter();
	StateRelatedTable *updateFilterState(FlowObject flowObject);
	void computeMeanMotionCenters();
	vector<StateRelatedTable *> getTables();
	vector<StateRelatedTable *> getTrajectoryCandidateTables();
	int getTablesCount();
	void tick();

private:
	uint64_t timer;
	vector<StateRelatedTable *> tables;
	StateRelatedTable *relateStateToTable(TrackedState *state);
	TrackedState *makeTrackedState(FlowObject flowObject);
	void deleteTable(StateRelatedTable *table);
	bool suppressionNeeded(StateRelatedTable *table);
	bool related(TrackedState *state, StateRelatedTable *table);
	bool relatedDisplacement(TrackedState *state, StateRelatedTable *table);
	bool relatedPrecise(TrackedState *state, StateRelatedTable *table);
	bool relatedPreciseMultipleStates(TrackedState *state, StateRelatedTable *table);
};

#endif /* STATEFULOBJECTFILTER_H_ */

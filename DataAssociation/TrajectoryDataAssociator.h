/*
 * TrajectoryDataAssociator.h
 *
 *  Created on: Jan 28, 2016
 *      Author: sled
 */

#ifndef TRAJECTORYDATAASSOCIATOR_H_
#define TRAJECTORYDATAASSOCIATOR_H_

#include "../OpticalLayer/StatefulObjectFilter.h"
#include "../Configuration/configs.h"
#include <map>

using namespace std;

typedef struct
{
	TrackedState *state;
	int tableIndex;
} DataAssociationState;

typedef struct
{
	bool associationPerformed;
	StateRelatedTable *dominatingCandidate;
	double associationPercentage;
} PairwiseDataAssociationResult;

class TrajectoryDataAssociator {
public:
	TrajectoryDataAssociator();
	virtual ~TrajectoryDataAssociator();
#ifdef DAS_ENABLE_MAP_ASSOCIAT
	void associateTrajectoryCandidatesInPlace(vector<StateRelatedTable *> trajectoryCandidates);
#endif
	vector<StateRelatedTable *> associateTrajectoryCandidates(vector<StateRelatedTable *> trajectoryCandidates);

private:
	StateRelatedTable *getLargestStateTable(vector<StateRelatedTable *> stateTables);
	int updateAssociabilityTable(int **dataAssociabilityTable, DataAssociationState das_i, DataAssociationState das_j);
	bool dataAssociationScoreAboveThreshold(StateRelatedTable *s, StateRelatedTable *t, int score, double *percent);
#ifdef DAS_ENABLE_MAP_ASSOCIAT
	vector<StateRelatedTable *> associate(map<StateRelatedTable *, vector<StateRelatedTable *>>);
#endif
	PairwiseDataAssociationResult directDataAssociation(StateRelatedTable *s, StateRelatedTable *t);
};

#endif /* TRAJECTORYDATAASSOCIATOR_H_ */

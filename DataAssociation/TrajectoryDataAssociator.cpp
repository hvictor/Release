/*
 * TrajectoryDataAssociator.cpp
 *
 *  Created on: Jan 28, 2016
 *      Author: sled
 */

#include "TrajectoryDataAssociator.h"
#include <math.h>

struct DASComparer {
	bool operator()(const StateRelatedTable* s, const StateRelatedTable* t) {
		return *s < *t;
	}
};

bool DASEqual(StateRelatedTable *s, StateRelatedTable *t)
{
	return (s->stateTableID == t->stateTableID);
}

TrajectoryDataAssociator::TrajectoryDataAssociator() {
	// TODO Auto-generated constructor stub

}

TrajectoryDataAssociator::~TrajectoryDataAssociator() {
	// TODO Auto-generated destructor stub
}

StateRelatedTable *TrajectoryDataAssociator::getLargestStateTable(vector<StateRelatedTable *> stateTables)
{
	StateRelatedTable *maxTable = stateTables[0];

	for (int i = 1; i < stateTables.size(); i++) {
		if (stateTables[i]->relatedStates.size() > maxTable->relatedStates.size()) {
			maxTable = stateTables[i];
		}
	}

	return maxTable;
}

// Update data associability table
int updateDataAssociabilityTable(int **dataAssociabilityTable, DataAssociationState das_i, DataAssociationState das_j)
{
	double deltax = fabsl(das_i.state->state.x - das_j.state->state.x);
	double deltay = fabsl(das_i.state->state.y - das_j.state->state.y);

	// The two points on the trajectory candidates can be associated
	// increment the score on the candidates-associability table
	if (sqrt(deltax*deltax + deltay*deltay) <= CFG_DATA_ASSOCIAT_SAMPLES_OFFSET) {
		dataAssociabilityTable[das_i.tableIndex][das_j.tableIndex] += 1;
		dataAssociabilityTable[das_j.tableIndex][das_i.tableIndex] += 1;
	}

	return dataAssociabilityTable[das_j.tableIndex][das_i.tableIndex];
}

// Data Association score check
bool TrajectoryDataAssociator::dataAssociationScoreAboveThreshold(StateRelatedTable *s, StateRelatedTable *t, int score, double *percent)
{
	StateRelatedTable *shortestTable = s;

	if (t->relatedStates.size() < shortestTable->relatedStates.size()) {
		shortestTable = t;
	}

	*percent = (double)score / (double)shortestTable->relatedStates.size();

	if (*percent >= CFG_DATA_ASSOCIAT_TRAJ_CORRESP_PERCENT)
	{
		return true;
	}

	return false;
}

// Perform data association
#ifdef DAS_ENABLE_MAP_ASSOCIAT
vector<StateRelatedTable *> associate(multimap<StateRelatedTable *, vector<StateRelatedTable *>>)
{
	vector<StateRelatedTable *> result;

	return result;
}
#endif

// Direct pairwise association
PairwiseDataAssociationResult TrajectoryDataAssociator::directDataAssociation(StateRelatedTable *s, StateRelatedTable *t)
{
	PairwiseDataAssociationResult pDAS_res;
	pDAS_res.associationPerformed = false;

	StateRelatedTable *maxTable = s;
	StateRelatedTable *minTable = t;

	if (maxTable->relatedStates.size() < minTable->relatedStates.size()) {
		maxTable = t;
		minTable = s;
	}

	double associationScore = 0.0;

	for (int i = 0; i < minTable->relatedStates.size(); i++) {
		double deltax = fabsl(minTable->relatedStates[i]->state.x - maxTable->relatedStates[i]->state.x);
		double deltay = fabsl(minTable->relatedStates[i]->state.y - maxTable->relatedStates[i]->state.y);

		// The two points on the trajectory candidates can be associated
		// increment the score on the candidates-associability table
		if (sqrt(deltax*deltax + deltay*deltay) <= CFG_DATA_ASSOCIAT_SAMPLES_OFFSET) {
			associationScore += 1.0;
		}
	}

	// If the two trajectory candidates are associated,
	if (dataAssociationScoreAboveThreshold(minTable, maxTable, (int)associationScore, &pDAS_res.associationPercentage)) {
		pDAS_res.associationPerformed = true;
		pDAS_res.dominatingCandidate = maxTable;
	}

	return pDAS_res;
}

// Associate slightly overlapping / aliased trajectories
vector<StateRelatedTable *> TrajectoryDataAssociator::associateTrajectoryCandidates(vector<StateRelatedTable *> trajectoryCandidates)
{
	vector<StateRelatedTable *> associatedData = trajectoryCandidates;

	bool dataAssociated = true;

	while (dataAssociated) {
		dataAssociated = false;
		// Perform pairwise data association
		for (vector<StateRelatedTable *>::iterator j = associatedData.begin(); j != associatedData.end() - 1; /* No inc */) {
			for (vector<StateRelatedTable *>::iterator k = j + 1; k != associatedData.end(); /* No inc */) {

				// Test (j, k) associability
				PairwiseDataAssociationResult pDAS = directDataAssociation(*j, *k);

				// If Data Association happened, replace j with the Data Association result, and erase k
				// Don't increment k, since everything slides left by one, just recheck with the new next that was shifted down
				if (pDAS.associationPerformed) {
					**j = *pDAS.dominatingCandidate;
					dataAssociated = true;
				}

				if (k != associatedData.end()) {
					k++;
				}
			}

			if (j != associatedData.end() - 1) {
				j++;
			}
		}
	}

	// Sort by table ID
	std::sort(associatedData.begin(), associatedData.end(), DASComparer());
	std::unique(associatedData.begin(), associatedData.end(), DASEqual);

	// Remove doubles from associated data

	return associatedData;
}

// In-Place: Associate slightly overlapping / aliased trajectories originated
// By the sparse optical flow algorithm
#ifdef DAS_ENABLE_MAP_ASSOCIAT
void TrajectoryDataAssociator::associateTrajectoryCandidatesInPlace(vector<StateRelatedTable *> trajectoryCandidates)
{
	if (trajectoryCandidates.size() <= 0)
		return;

	// Candidates associability table
	int **associabilityTable = (int **)malloc(trajectoryCandidates.size() * sizeof(int *));
	for (int i = 0; i < trajectoryCandidates.size(); i++) {
		associabilityTable[i] = (int *)malloc(trajectoryCandidates.size() * sizeof(int));
		bzero(associabilityTable[i], trajectoryCandidates.size() * sizeof(int));
	}

	StateRelatedTable *maxTable = getLargestStateTable(trajectoryCandidates);
	int statesN = maxTable->relatedStates.size();


	for (int i = 0; i < statesN; i++) {

		// Prepare vertical window with states at position (i < statesN) in tables, if they are there
		vector<DataAssociationState> verticalWindow;

		for (int j = 0; j < trajectoryCandidates.size(); j++) {
			if (i < trajectoryCandidates[j]->relatedStates.size()) {
				vector<TrackedState *> trackedStates = trajectoryCandidates[j]->relatedStates;

				DataAssociationState das;
				das.state = trackedStates[i];
				das.tableIndex = j;

				verticalWindow.push_back(das);
			}
		}

		// Mutual comparison
		for (int k = 0; k < verticalWindow.size() - 1; k++) {
			for (int l = k + 1; l < verticalWindow.size(); l++) {
				int score = updateAssociabilityTable(associabilityTable, verticalWindow[k], verticalWindow[l]);
			}
		}
	}

	// Relation table
	map<StateRelatedTable *, vector<StateRelatedTable *>> associationMap;

	for (int i = 0; i < trajectoryCandidates.size() - 1; i++) {
		for (int j = i + 1; j < trajectoryCandidates.size(); j++) {
			double percent;
			if (dataAssociationScoreAboveThreshold(trajectoryCandidates[i], trajectoryCandidates[j], associabilityTable[i][j], &percent)) {
				associationMap[trajectoryCandidates[i]].push_back(trajectoryCandidates[j]);
			}
		}
	}

	vector<StateRelatedTable *> result = associate(associationMap);

	// Free score matrix memory
	for (int i = 0; i < trajectoryCandidates.size(); i++) {
		free(associabilityTable[i]);
	}
	free(associabilityTable);

}
#endif

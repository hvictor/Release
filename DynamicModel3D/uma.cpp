#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <list>
#include <algorithm>
#include "../Common/data_types.h"
#include "DynamicModel.h"
#include "../Configuration/Configuration.h"
#include "../StaticModel/GroundModel.h"
#include "../RealTime/nanotimer_rt.h"

using namespace std;

#define UMA_BUFFER_SIZE		10

static int _idx = 0;
static dyn_state_t *_buffer[UMA_BUFFER_SIZE];
static Configuration *_config;
static GroundModel *_groundModel;

void uma_init()
{
	_config = Configuration::getInstance();
	_groundModel = GroundModel::getInstance();
}

// Least-squares linear regression estimator

void uma_reset()
{
	for (int i = 0; i < UMA_BUFFER_SIZE; i++) {
		_buffer[i] = 0;
		_idx = 0;
	}
}

void uma_buffer_shift()
{
	for (int i = 1; i <= _idx; i++) {
		_buffer[i - 1] = _buffer[i];
	}
}

dyn_state_t *uma_analyze_buffer_data()
{
	dyn_state_t *impact_state = _buffer[UMA_BUFFER_SIZE / 2];
	dyn_state_t *initial_state = _buffer[0];

	// If an impact state has not yet shifted to the central position, return the control
	if (!impact_state->impacted_2D) {
		return 0;
	}

	// Impact shifted to the buffer central position, perform linear regression analysis
	// Compute delta t and mean velocity during descent
	double delta_ms = nanotimer_rt_ms_diff(&(initial_state->t), &(impact_state->t));

	double vy_mean = 0.0;
	for (int i = 0; i <= UMA_BUFFER_SIZE / 2; i++) {
		vy_mean += _buffer[i]->vy;
	}
	vy_mean /= (double)(UMA_BUFFER_SIZE/2 + 1);

	// Approximate the y position
	impact_state->pos.y = (initial_state->pos.y + (vy_mean * delta_ms));

	// Compute the impact state distance from the ground model
	if (_groundModel->computeDistanceFromGroundPlane(impact_state->pos) <= _config->dynamicModelParameters.impactMaxFloorDistance) {
		impact_state->impacted = true;
		return impact_state;
	}

	return 0;
}

dyn_state_t *uma_update(dyn_state_t *state)
{
	if (_idx < (UMA_BUFFER_SIZE - 1)) {
		_buffer[_idx++] = state;
		return 0;
	}

	// Free last buffer position, _idx points to the last buffer position
	uma_buffer_shift();

	// Overwrite the last buffer position
	_buffer[_idx] = state;

	// Before performing UMA analysis, run the relaxed impact-check algorithm
	dyn_state_t *actual = state;
	dyn_state_t *prev = _buffer[_idx - 1];
	if (prev->vy >= 0.0 && actual->vy < 0.0) {
		double dist = _groundModel->computeDistanceFromGroundPlane(prev->pos);

		if (dist <= _config->dynamicModelParameters.impactMaxFloorDistance) {
			prev->impacted = true;
		}

		return prev;
	}

	// Perform buffered data analysis
	dyn_state_t *res = uma_analyze_buffer_data();

	return res;
}

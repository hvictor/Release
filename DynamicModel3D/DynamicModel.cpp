/*
 * DynamicModel.cpp
 *
 *  Created on: May 28, 2016
 *      Author: sled
 */

#include "DynamicModel.h"

DynamicModel *DynamicModel::getInstance()
{
	static DynamicModel *inst = 0;

	if (inst == 0) {
		inst = new DynamicModel();
	}

	return inst;
}

DynamicModel::DynamicModel()
{
	kalmanFilter = new DynamicModel3D_KalmanFilter();
	filterCallCounter = 0;
	config = Configuration::getInstance();
}

DynamicModel::~DynamicModel()
{
}

void DynamicModel::compute_dynamical_state_data(dyn_state_t *actual, dyn_state_t *prev)
{
	struct timespec *prev_t = &(prev->t);
	struct timespec *act_t = &(actual->t);

	double delta_ms = nanotimer_rt_ms_diff(prev_t, act_t);

	// Velocity components [mm / ms] = [m/s]
	actual->vx = (actual->pos.x - prev->pos.x) / delta_ms;
	actual->vy = (actual->pos.y - prev->pos.y) / delta_ms;
	actual->vz = (actual->pos.z - prev->pos.z) / delta_ms;

	// Velocity in [mm / ms] is already a value in [m / s]
	printf("Dynamic Model :: compute_dynamical_state_data() :: delta t = %.2f [ms]        v = [%.2f, %.2f, %.2f] [m / s]\n",
			delta_ms, actual->vx, actual->vy, actual->vz);
}

void DynamicModel::recalc(Vector3D v, struct timespec t)
{
	if (config->dynamicModelParameters.useInputKalmanFilter) {
		if (filterCallCounter == 0) {
			printf("Dynamic Model :: Kalman Filter :: Initializing filter.\n");
			kalmanFilter->initParameters(v.x, v.y, v.z);
			filterCallCounter = 1;
		}

		printf("Dynamic Model :: Kalman Filter :: MEAS=[%.2f %.2f %.2f] ", v.x, v.y, v.z);

		kalmanFilter->recalc(filterCallCounter++, v.x, v.y, v.z, &v.x, &v.y, &v.z);

		printf("FILT=[%.2f %.2f %.2f]\n", v.x, v.y, v.z);
	}

	// Make new dynamical state
	dyn_state_t *dyn = make_dynamical_state(v, t);

	// Compute velocity and acceleration components based on previous state

	if(state.size() > 1) {
		compute_dynamical_state_data(dyn, *(state.begin()));
	}
	else {
		dyn->vx = 0.0;
		dyn->vy = 0.0;
		dyn->vz = 0.0;
	}

	// Add dynamical state to the model
	state.push_front(dyn);
}

dyn_state_t *DynamicModel::make_dynamical_state(Vector3D v, struct timespec t)
{
	dyn_state_t *dyn_state = new dyn_state_t();

	dyn_state->pos = v;
	dyn_state->t = t;

	return dyn_state;
}

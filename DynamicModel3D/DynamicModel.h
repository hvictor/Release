/*
 * DynamicModel.h
 *
 *  Created on: May 28, 2016
 *      Author: sled
 */

#ifndef DYNAMICMODEL_H_
#define DYNAMICMODEL_H_

#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <list>
#include <algorithm>
#include "DynamicModel3D_KalmanFilter.h"
#include "../Common/opencv_headers.h"
#include "../Common/data_types.h"
#include "../RealTime/nanotimer_rt.h"
#include "../Configuration/Configuration.h"
#include "../StaticModel/GroundModel.h"

using namespace std;
using namespace cv;

typedef struct
{
	Vector3D pos;		// Real-world position

	double ax, ay, az;	// Accelerations
	double vx, vy, vz;	// Velocities

	struct timespec t;	// Instant

	bool impacted;		// Impact status
} dyn_state_t;

typedef struct
{
	bool impact;
	Vector3D impact_pos;
} dyn_model_result_t;

class DynamicModel {
public:
	virtual ~DynamicModel();
	dyn_model_result_t recalc(Vector3D v, struct timespec t);
	void UMA_signal_impact_2D();
	void UMA_reset();
	static DynamicModel *getInstance();

private:
	DynamicModel();
	list<dyn_state_t *> state;
	DynamicModel3D_KalmanFilter *kalmanFilter;
	int filterCallCounter;
	Configuration *config;
	dyn_state_t *make_dynamical_state(Vector3D v, struct timespec t);
	void compute_dynamical_state_data(dyn_state_t *actual, dyn_state_t *prev, dyn_model_result_t *res);
	GroundModel *_groundModel;
};

#endif /* DYNAMICMODEL_H_ */

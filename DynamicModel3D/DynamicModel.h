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
#include "../Common/opencv_headers.h"

using namespace std;
using namespace cv;

#include "DynamicModel3D.h"
#include "../RealTime/nanotimer_rt.h"
#include "../Configuration/Configuration.h"

typedef struct
{
	Vector3D pos;		// Real-world position

	double ax, ay, az;	// Accelerations
	double vx, vy, vz;	// Velocities

	struct timespec t;	// Instant

	bool impacted;		// Impact status
} dyn_state_t;

class DynamicModel {
public:
	virtual ~DynamicModel();
	void recalc(Vector3D v, struct timespec t);
	static DynamicModel *getInstance();

private:
	DynamicModel();
	list<dyn_state_t *> state;
	DynamicModel3D_KalmanFilter *kalmanFilter;
	int filterCallCounter;
	Configuration *config;
	dyn_state_t *make_dynamical_state(Vector3D v, struct timespec t);
	void compute_dynamical_state_data(dyn_state_t *actual, dyn_state_t *prev);
};

#endif /* DYNAMICMODEL_H_ */

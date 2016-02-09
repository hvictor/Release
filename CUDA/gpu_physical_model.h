/*
 * gpu_physical_model.h
 *
 *  Created on: Jan 24, 2016
 *      Author: sled
 */

#ifndef GPU_PHYSICAL_MODEL_H_
#define GPU_PHYSICAL_MODEL_H_

// GPU physical model computation
void gpu_physical_model_compute(double g, double K, double vx_0, double vy_0, double vz_0, double *x, double *y, double *z, int N);

#endif /* GPU_PHYSICAL_MODEL_H_ */

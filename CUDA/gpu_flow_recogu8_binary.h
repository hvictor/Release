/*
 * gpu_flow_recogu8_binary.h
 *
 *  Created on: Jan 27, 2016
 *      Author: sled
 */

#ifndef GPU_FLOW_RECOGU8_BINARY_H_
#define GPU_FLOW_RECOGU8_BINARY_H_

void gpu_flow_recogu8_binary(float *flowx, float *flowy, unsigned char *u8res, float thresh, int N);

#endif /* GPU_FLOW_RECOGU8_BINARY_H_ */

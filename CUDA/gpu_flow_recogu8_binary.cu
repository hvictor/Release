/*
 * cuda_flow_recog8u_binary.c
 *
 *  Created on: Jan 27, 2016
 *      Author: sled
 */

//
// Threshold-based Contrasting GPU implementation
//
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define THREADS_PER_BLOCK		128

// CUDA kernel declaration
__global__ void cuda_flow_binary_kernel(float *flowx, float *flowy, unsigned char *u8res, float thresh, int N);

// C/C++ Wrapper
void gpu_flow_recogu8_binary(float *flowx, float *flowy, unsigned char *u8res, float thresh, int N)
{
	// Device memory
	float *dev_flowx;
	float *dev_flowy;
	uint8_t *dev_u8res;

	// Allocated device memory
	cudaMalloc((void **)&dev_flowx, N * sizeof(float));
	cudaMalloc((void **)&dev_flowy, N * sizeof(float));
	cudaMalloc((void **)&dev_u8res, N * sizeof(uint8_t));

	// Upload data to device memory
	cudaMemcpy(dev_flowx, flowx, N * sizeof(float), cudaMemcpyHostToDevice);
	cudaMemcpy(dev_flowy, flowy, N * sizeof(float), cudaMemcpyHostToDevice);

	cuda_flow_binary_kernel<<<((N + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK), THREADS_PER_BLOCK>>>(dev_flowx, dev_flowy, dev_u8res, thresh, N);

	cudaMemcpy(u8res, dev_u8res, N * sizeof(uint8_t), cudaMemcpyDeviceToHost);

	cudaFree(dev_flowx);
	cudaFree(dev_flowy);
}

// u8res contains a binary map of optical flow regions with motion intensity over the threshold
__global__ void cuda_flow_binary_kernel(float *flowx, float *flowy, unsigned char *u8res, float thresh, int N)
{
	int idx = threadIdx.x + (blockDim.x *blockIdx.x);

	if (idx < N) {

		if (isnan(flowx[idx]) || isnan(flowy[idx]) || fabs(flowx[idx]) < 1e9 || fabs(flowy[idx]) < 1e9)
			return;

		if (sqrt(flowx[idx]*flowx[idx] + flowy[idx]*flowy[idx]) >= thresh) {
			u8res[idx] = 1;
		}
		else {
			u8res[idx] = 0;
		}
	}
}







/*
 * gpu_physical_model.c
 *
 *  Created on: Jan 24, 2016
 *      Author: sled
 */

//
// Physical model GPU implementation
//
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#define THREADS_PER_BLOCK		128

// CUDA kernel declaration
__global__ void cuda_physical_model_kernel(double g, double K, double vx_0, double vy_0, double vz_0, double *x, double *y, double *z, int N);

// C/C++ Wrapper
void gpu_physical_model_compute(double g, double K, double vx_0, double vy_0, double vz_0, double *x, double *y, double *z, int N)
{
	// Device memory
	double *dev_x;
	double *dev_y;
	double *dev_z;

	size_t size = N * sizeof(double);

	// Allocated device memory
	cudaMalloc((void **)&dev_x, size);
	cudaMalloc((void **)&dev_y, size);
	cudaMalloc((void **)&dev_z, size);

	// Upload data to device memory
	cudaMemcpy(dev_x, x, size, cudaMemcpyHostToDevice);
	cudaMemcpy(dev_y, y, size, cudaMemcpyHostToDevice);
	cudaMemcpy(dev_z, z, size, cudaMemcpyHostToDevice);

	cuda_physical_model_kernel<<<((N + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK), THREADS_PER_BLOCK>>>(g, K, vx_0, vy_0, vz_0, dev_x, dev_y, dev_z, N);

	cudaMemcpy(x, dev_x, size, cudaMemcpyDeviceToHost);
	cudaMemcpy(y, dev_y, size, cudaMemcpyDeviceToHost);
	cudaMemcpy(z, dev_z, size, cudaMemcpyDeviceToHost);

	cudaFree(dev_x);
	cudaFree(dev_y);
	cudaFree(dev_z);
}

// GPU memory buffers are filled with computed physical states 3D components
__global__ void cuda_physical_model_kernel(double g, double K, double vx_0, double vy_0, double vz_0, double *x, double *y, double *z, int N)
{
	int idx = threadIdx.x + (blockDim.x *blockIdx.x);

	if (idx == 0)
		return;

	if (idx < N) {
		x[idx] = x[0] + (vx_0 / K) * (1.0 - exp(-K * idx)/K);
		y[idx] = y[0] + ((g + K * vy_0) / (K * K)) * (1.0 - exp(-K * idx)) - (g * idx) / K;
		z[idx] = z[0] + (vz_0 / K) * (1.0 - exp(-K * idx)/K);
	}
}




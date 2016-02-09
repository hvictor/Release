//
// Threshold-based Contrasting GPU implementation
//
#include <unistd.h>
#include <stdlib.h>

#define THREADS_PER_BLOCK		128

// CUDA kernel declaration
__global__ void cuda_contrast_kernel(unsigned char *u8data, unsigned char *u8res, unsigned int N, unsigned int thresh, unsigned int delta);

// C/C++ Wrapper
unsigned char *gpu_contrastu8(unsigned char *u8data, unsigned char *u8res, unsigned int N, unsigned int thresh, unsigned int delta)
{
	// Host memory
	unsigned char *host_u8res;

	// Device memory
	unsigned char *dev_u8data;
	unsigned char *dev_u8res;

	size_t size = N * sizeof(unsigned char);
	host_u8res = (unsigned char *)malloc(size);

	// Allocated device memory
	cudaMalloc((void **)&dev_u8data, size);
	cudaMalloc((void **)&dev_u8res, size);

	// Upload data to device memory
	cudaMemcpy(dev_u8data, u8data, size, cudaMemcpyHostToDevice);

	cuda_contrast_kernel<<<((N + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK), THREADS_PER_BLOCK>>>(dev_u8data, dev_u8res, N, thresh, delta);

	cudaMemcpy(host_u8res, dev_u8res, size, cudaMemcpyDeviceToHost);

	cudaFree(dev_u8data);
	cudaFree(dev_u8res);

	return host_u8res;
}

// CUDA kernel
__global__ void cuda_contrast_kernel(unsigned char *u8data, unsigned char *u8res, unsigned int N, unsigned int thresh, unsigned int delta)
{
	int idx = threadIdx.x + (blockDim.x *blockIdx.x);

	if (idx < N) {

		if (u8data[idx] >= thresh) {
			if (u8data[idx] + delta >= 255) {
				u8res[idx] = 255;
			}
			else {
				u8res[idx] = u8data[idx] + delta;
			}
		}
		else {
			if (u8data[idx] - delta <= 0) {
				u8res[idx] = 0;
			}
			else {
				u8res[idx] = u8data[idx] - delta;
			}
		}
	}
}

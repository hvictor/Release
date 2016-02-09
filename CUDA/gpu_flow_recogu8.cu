//
// Threshold-based Contrasting GPU implementation
//
#include <unistd.h>
#include <stdlib.h>

#define THREADS_PER_BLOCK		128

// CUDA kernel declaration
__global__ void cuda_flow_bitmap_kernel(unsigned char *u8data, unsigned char *u8res, unsigned int N, unsigned int thresh);

// C/C++ Wrapper
unsigned char *gpu_flow_recogu8_bitmap(unsigned char *u8data, unsigned char *u8res, unsigned int N, unsigned int thresh)
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

	cuda_flow_bitmap_kernel<<<((N + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK), THREADS_PER_BLOCK>>>(dev_u8data, dev_u8res, N, thresh);

	cudaMemcpy(host_u8res, dev_u8res, size, cudaMemcpyDeviceToHost);

	cudaFree(dev_u8data);
	cudaFree(dev_u8res);

	return host_u8res;
}

// u8res contains a binary map of optical flow regions with motion intensity over the threshold
__global__ void cuda_flow_bitmap_kernel(unsigned char *u8data, unsigned char *u8res, unsigned int N, unsigned int thresh)
{
	int idx = threadIdx.x + (blockDim.x *blockIdx.x);

	if (idx < N) {

		if (u8data[idx] >= thresh) {
			u8res[idx] = 1;
		}
		else {
			u8res[idx] = 0;
		}
	}
}




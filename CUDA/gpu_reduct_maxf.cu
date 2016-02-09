/*
 * gpu_maxf.c
 *
 *  Created on: Jan 24, 2016
 *      Author: sled
 */

__device__ float atomicMaxf(float* address, float val)
{
	int *address_as_int =(int*)address;
	int old = *address_as_int, assumed;
	while (val > __int_as_float(old)) {
		assumed = old;
		old = atomicCAS(address_as_int, assumed,
				__float_as_int(val));
	}
	return __int_as_float(old);
}


__global__ void cuda_reduct_maxf_kernel(const float* const d_array, float* d_max,  const size_t elements)
{
	extern __shared__ float shared[];

	int tid = threadIdx.x;
	int gid = (blockDim.x * blockIdx.x) + tid;
	shared[tid] = -FLOAT_MAX;

	while (gid < elements) {
		shared[tid] = max(shared[tid], d_array[gid]);
		gid += gridDim.x*blockDim.x;
	}
	__syncthreads();
	gid = (blockDim.x * blockIdx.x) + tid;  // 1
	for (unsigned int s=blockDim.x/2; s>0; s>>=1)
	{
		if (tid < s && gid < elements)
			shared[tid] = max(shared[tid], shared[tid + s]);
		__syncthreads();
	}

	if (tid == 0)
		atomicMaxf(d_max, shared[0]);
}



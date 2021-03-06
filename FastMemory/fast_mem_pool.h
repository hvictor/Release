/*
 * fast_mem_pool.h
 *
 *  Created on: Jan 26, 2016
 *      Author: sled
 */

#ifndef FAST_MEM_POOL_H_
#define FAST_MEM_POOL_H_

#include "../Configuration/configs.h"
#include "../Common/opencv_headers.h"
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "../RealTime/nanotimer_rt.h"


typedef struct
{
	uint8_t *left_data;
	uint8_t *right_data;
	float *depth_data;
	float *xyz_data;
	float *confidence_data;
	bool depth_data_avail;
	int index;
	int free;
	int step_xyz;
	int step_confidence;
	int step_depth;
	//Mat *xyzMat;
	int frame_counter;

	struct timespec t;
} FrameData;

void fast_mem_pool_init(int frame_width, int frame_height, int channels);
FrameData *fast_mem_pool_fetch_memory(void);
void fast_mem_pool_release_memory(FrameData *pFrameData);

#endif /* FAST_MEM_POOL_H_ */

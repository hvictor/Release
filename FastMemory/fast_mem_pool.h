/*
 * fast_mem_pool.h
 *
 *  Created on: Jan 26, 2016
 *      Author: sled
 */

#ifndef FAST_MEM_POOL_H_
#define FAST_MEM_POOL_H_

#include "../Configuration/configs.h"
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct
{
	uint8_t *left_data;
	uint8_t *right_data;
	uint8_t *depth_data;
	int index;
	int free;
} FrameData;

void fast_mem_pool_init(int frame_width, int frame_height, int channels);
FrameData *fast_mem_pool_fetch_memory(void);
void fast_mem_pool_release_memory(FrameData *pFrameData);

#endif /* FAST_MEM_POOL_H_ */

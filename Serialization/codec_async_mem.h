/*
 * codec_async_mem.h
 *
 *  Created on: Jun 22, 2016
 *      Author: sled
 */

#ifndef CODEC_ASYNC_MEM_H_
#define CODEC_ASYNC_MEM_H_

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../RealTime/nanotimer_rt.h"


typedef struct
{
	void *data;
	int index;
} codec_buffer_t;

void codec_async_mem_init(int frame_width, int frame_height, int channels);
codec_buffer_t *codec_async_mem_fetch_memory(void);
void codec_async_mem_release_memory(codec_buffer_t *p_buf);

#endif /* CODEC_ASYNC_MEM_H_ */

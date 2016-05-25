/*
 * fast_mem_pool.cpp
 *
 *  Created on: Jan 26, 2016
 *      Author: sled
 */

#include "fast_mem_pool.h"
#include "../Configuration/Configuration.h"
#include <pthread.h>
#include <semaphore.h>


static pthread_spinlock_t mem_spin;
static sem_t sem_empty;
static int frame_buffer_size;

volatile int mem_count;
static FrameData *mem_head;
static FrameData *mem_tail;
static FrameData *mem;

void fast_mem_pool_init(int frame_width, int frame_height, int channels)
{
	frame_buffer_size = Configuration::getInstance()->getOpticalLayerParameters().frameBufferSize;
	pthread_spin_init(&mem_spin, 0);

	mem = (FrameData *)malloc(frame_buffer_size * sizeof(FrameData));

	for (int i = 0 ; i < frame_buffer_size; i++) {
		mem[i].left_data = (uint8_t *)malloc(frame_width * frame_height * channels * sizeof(uint8_t));

		if (Configuration::getInstance()->getOperationalMode().inputDevice != MonoCameraVirtual) {
			mem[i].right_data = (uint8_t *)malloc(frame_width * frame_height * channels * sizeof(uint8_t));
			mem[i].xyz_data = (float *)malloc(frame_width * frame_height * sizeof(float));
			mem[i].confidenceMat(Size(frame_width, frame_height), CV_32FC1);
			//mem[i].confidence_data = (float *)malloc(frame_width * frame_height * sizeof(float));
			//mem[i].depth_data = (uint8_t *)malloc(frame_width * frame_height * sizeof(uint8_t));
		}

		mem[i].depth_data_avail = false;
	}

	mem_count = 0;
	mem_head = mem;
	mem_tail = mem;

	sem_init(&sem_empty, 0, frame_buffer_size);
}

FrameData *fast_mem_pool_fetch_memory(void)
{
	FrameData *ret;

	sem_wait(&sem_empty);

	pthread_spin_lock(&mem_spin);

	ret = mem_head;

	if (mem_head == &(mem[frame_buffer_size-1])) {
		mem_head = &(mem[0]);
	}
	else {
		mem_head++;
	}

	mem_count--;

	pthread_spin_unlock(&mem_spin);

	return ret;
}

void fast_mem_pool_release_memory(FrameData *pFrameData)
{
	pthread_spin_lock(&mem_spin);

	*mem_tail = *pFrameData;


	if (mem_tail == &(mem[frame_buffer_size-1])) {
		mem_tail = &(mem[0]);
	}
	else {
		mem_tail++;
	}

	mem_count++;

	sem_post(&sem_empty);

	pthread_spin_unlock(&mem_spin);
}

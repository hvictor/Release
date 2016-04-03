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
		}
	}

	mem_count = 0;
	mem_head = mem;
	mem_tail = mem;

	sem_init(&sem_empty, 0, frame_buffer_size);

	printf("Fast memory init ok\n");
}

FrameData *fast_mem_pool_fetch_memory(void)
{
	FrameData *ret;

	printf("gaining sem_empty...\n");
	sem_wait(&sem_empty);
	printf("gaining sem_empty OK\n");

	pthread_spin_lock(&mem_spin);

	ret = mem_head;

	if (mem_head == &(mem[frame_buffer_size-1])) {
		printf("setting mem_head = mem[0]\n");
		mem_head = &(mem[0]);
		printf("OK: setting mem_head = mem[0]\n");
	}
	else {
		printf("setting mem_head++\n");
		mem_head++;
		printf("OK setting mem_head++\n");
	}

	mem_count--;

	pthread_spin_unlock(&mem_spin);

	printf("Returning ret = %p\n", ret);

	return ret;
}

void fast_mem_pool_release_memory(FrameData *pFrameData)
{
	pthread_spin_lock(&mem_spin);

	printf("pushing mem...\n");
	*mem_tail = *pFrameData;
	printf("mem pushed\n");

	if (mem_tail == &(mem[frame_buffer_size-1])) {
		mem_tail = &(mem[0]);
	}
	else {
		mem_tail++;
	}

	mem_count++;

	printf("posting sem empty...\n");
	sem_post(&sem_empty);
	printf("post sem empty ok\n");

	pthread_spin_unlock(&mem_spin);
}

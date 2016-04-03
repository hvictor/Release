/*
 * fast_mem_pool.cpp
 *
 *  Created on: Jan 26, 2016
 *      Author: sled
 */

#include "fast_mem_pool.h"
#include "../Configuration/Configuration.h"
#include <pthread.h>


static pthread_spinlock_t spin;
static sem_t empty;
static sem_t full;
static int frame_buffer_size;

volatile int count;
static FrameData *head;
static FrameData *tail;
static FrameData *mem;

void fast_mem_pool_init(int frame_width, int frame_height, int channels)
{
	frame_buffer_size = Configuration::getInstance()->getOpticalLayerParameters().frameBufferSize;
	pthread_spin_init(&spin, 0);

	mem = (FrameData *)malloc(frame_buffer_size * sizeof(FrameData));

	for (int i = 0 ; i < frame_buffer_size; i++) {
		mem[i].left_data = (uint8_t *)malloc(frame_width * frame_height * channels * sizeof(uint8_t));

		if (Configuration::getInstance()->getOperationalMode().inputDevice != MonoCameraVirtual) {
			mem[i].right_data = (uint8_t *)malloc(frame_width * frame_height * channels * sizeof(uint8_t));
		}

		mem[i].index = i;
		mem[i].free = 1;
	}

	count = 0;
	head = mem;
	tail = mem;

	sem_init(&empty, 0, 0);
	sem_init(&full, 0, 1);
}

FrameData *fast_mem_pool_fetch_memory(void)
{
	FrameData *ret;

	sem_wait(&empty);

	pthread_spin_lock(&spin);

	ret = head;

	if (head == mem[frame_buffer_size-1]) {
		head = mem[0];
	}
	else {
		head++;
	}

	count--;
	sem_post(&full);

	pthread_spin_unlock(&spin);

	return ret;
}

void fast_mem_pool_release_memory(FrameData *pFrameData)
{
	printf("fast_mem_pool: trying push (release)...\n");
	sem_wait(&full);
	printf("fast_mem_pool: OK push (release)!\n");

	pthread_spin_lock(&spin);

	*tail = *pFrameData;

	if (tail == mem[frame_buffer_size-1]) {
		tail = mem[0];
	}
	else {
		tail++;
	}

	count++;

	sem_post(&empty);

	pthread_spin_unlock(&spin);
}

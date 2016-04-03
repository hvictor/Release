/*
 * fast_mem_pool.cpp
 *
 *  Created on: Jan 26, 2016
 *      Author: sled
 */

#include "fast_mem_pool.h"
#include "../Configuration/Configuration.h"
#include <pthread.h>


static FrameData *mem[FAST_MEM_POOL_FRAMES_MAX];

static int used = 0;

static pthread_spinlock_t spinlock;
volatile int last_free_index = 0;

void fast_mem_pool_init(int frame_width, int frame_height, int channels)
{
	pthread_spin_init(&spinlock, 0);

	used = 0;
	last_free_index = 0;

	for (int i = 0 ; i < FAST_MEM_POOL_FRAMES_MAX; i++) {
		mem[i] = (FrameData *)malloc(sizeof(FrameData));
		mem[i]->left_data = (uint8_t *)malloc(frame_width * frame_height * channels * sizeof(uint8_t));

		if (Configuration::getInstance()->getOperationalMode().inputDevice != MonoCameraVirtual) {
			mem[i]->right_data = (uint8_t *)malloc(frame_width * frame_height * channels * sizeof(uint8_t));
		}

		mem[i]->index = i;
		mem[i]->free = 1;
	}
}

FrameData *fast_mem_pool_fetch_memory(void)
{
	FrameData *pFrameData = NULL;

	pthread_spin_lock(&spinlock);

	if (used == FAST_MEM_POOL_FRAMES_MAX) {
		pthread_spin_unlock(&spinlock);
		return NULL;
	}

	if (last_free_index < 0)
	{
		return NULL;
	}

	pFrameData = mem[last_free_index];

	last_free_index++;

	if (last_free_index >= FAST_MEM_POOL_FRAMES_MAX) {
		for (int i = 0; i < FAST_MEM_POOL_FRAMES_MAX; i++) {
			if (mem[i]->free) {
				last_free_index = i;
				break;
			}
		}
	}

	// STABLE version:
	/*
	for (int i = 0; i < FAST_MEM_POOL_FRAMES_MAX; i++) {
		if (mem[i]->free) {
			pFrameData = mem[i];
			break;
		}
	}
	*/

	pFrameData->free = 0;
	used++;

	pthread_spin_unlock(&spinlock);

	return pFrameData;
}

void fast_mem_pool_release_memory(FrameData *pFrameData)
{
	pthread_spin_lock(&spinlock);

	// Memory not cleaned up for performance reasons: it will anyway be overwritten by the next frame data allocated
	mem[pFrameData->index]->free = 1;

	// If index < (used - 1): last_free_index = index
	if (pFrameData->index < (used - 1)) {
		last_free_index = pFrameData->index;
	}
	else {
		last_free_index = (used - 1);
	}

	used--;

	pthread_spin_unlock(&spinlock);
}

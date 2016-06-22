/*
 * codec_async_mem.cpp
 *
 *  Created on: Jun 22, 2016
 *      Author: sled
 */

#include "codec_async_mem.h"
#include "../Configuration/Configuration.h"
#include <pthread.h>
#include <semaphore.h>

static pthread_spinlock_t _mem_spin;
static sem_t _sem_empty;
static int _codec_mem_pool_size;
static volatile int _mem_count;

static codec_buffer_t *_mem_head;
static codec_buffer_t *_mem_tail;
static codec_buffer_t *mem;

void codec_async_mem_init(int frame_width, int frame_height, int channels)
{
	_codec_mem_pool_size = 80;
	pthread_spin_init(&_mem_spin, 0);

	mem = (codec_buffer_t *)malloc(_codec_mem_pool_size * sizeof(codec_buffer_t));

	for (int i = 0 ; i < _codec_mem_pool_size; i++) {
		mem[i].index = i;
		mem[i].data = (void *)malloc(	frame_width * frame_height * channels * sizeof(uint8_t) +	// Reference Camera Frame, UINT8, 4 channels
										sizeof(short) +												// Depth Data availability flag
										frame_width * frame_height * sizeof(float) +				// Depth Data, float, 1 channels
										sizeof(int) +												// Depth Data aligned representation step offset
										sizeof(int)													// Frame Counter
									);

		);

	}

	_mem_count = 0;
	_mem_head = mem;
	_mem_tail = mem;

	sem_init(&_sem_empty, 0, _codec_mem_pool_size);
}

codec_buffer_t *codec_async_mem_fetch_memory(void)
{
	codec_buffer_t *ret;

	sem_wait(&_sem_empty);

	pthread_spin_lock(&_mem_spin);

	ret = _mem_head;

	if (_mem_head == &(mem[_codec_mem_pool_size-1])) {
		_mem_head = &(mem[0]);
	}
	else {
		_mem_head++;
	}

	_mem_count--;

	pthread_spin_unlock(&_mem_spin);

	return ret;
}

void codec_async_mem_release_memory(codec_buffer_t *p_buf)
{
	pthread_spin_lock(&_mem_spin);

	*_mem_tail = *p_buf;


	if (_mem_tail == &(mem[_codec_mem_pool_size-1])) {
		_mem_tail = &(mem[0]);
	}
	else {
		_mem_tail++;
	}

	_mem_count++;

	sem_post(&_sem_empty);

	pthread_spin_unlock(&_mem_spin);
}

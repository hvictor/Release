#ifndef __A_SPINLOCK_QUEUE_H
#define __A_SPINLOCK_QUEUE_H

#include "../FastMemory/fast_mem_pool.h"

#define QUEUE_MAX 400

struct node
{
	void *data;
};

typedef struct
{
	pthread_spinlock_t spin;

	int count;
	struct node *head;
	struct node *tail;

	struct node a[QUEUE_MAX];
} SpinlockQueue;

void array_spinlock_queue_init(SpinlockQueue *q);
int array_spinlock_queue_push(SpinlockQueue *q, void *data);
int array_spinlock_queue_pull(SpinlockQueue *q, void **data_dest);

#endif

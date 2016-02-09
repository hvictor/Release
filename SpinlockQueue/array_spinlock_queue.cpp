#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "array_spinlock_queue.h"

void array_spinlock_queue_init(SpinlockQueue *q)
{
	q->count = 0;
	q->head = q->a;
	q->tail = q->a;

	pthread_spin_init(&q->spin, 0);
}

int array_spinlock_queue_push(SpinlockQueue *q, void *data)
{
	pthread_spin_lock(&q->spin);

	if (q->count >= QUEUE_MAX) {
		pthread_spin_unlock(&q->spin);
		return -1;
	}

	q->tail->data = data;
	
	if (q->tail == &(q->a[QUEUE_MAX-1])) {
		q->tail = &(q->a[0]);
	}
	else {
		q->tail++;
	}
	
	q->count++;

	pthread_spin_unlock(&q->spin);

	return 0;
}

int array_spinlock_queue_pull(SpinlockQueue *q, void **data_dest)
{
	pthread_spin_lock(&q->spin);

	if (!q->count) {
		pthread_spin_unlock(&q->spin);
		return -1;
	}

	*data_dest = q->head->data;

	if (q->head == &(q->a[QUEUE_MAX-1])) {
		q->head = &(q->a[0]);
	}
	else {
		q->head++;
	}

	q->count--;

	pthread_spin_unlock(&q->spin);
	
	return 0;
}

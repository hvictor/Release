// Spinlock Queue
// Victor Huerlimann

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "array_spinlock_queue.h"
#include "../Configuration/Configuration.h"
#include "../RealTime/nanotimer_rt.h"

struct timespec s, t;

void array_spinlock_queue_init(SpinlockQueue *q)
{
	q->count = 0;
	q->head = q->a;
	q->tail = q->a;

	pthread_spin_init(&q->spin, 0);

	sem_init(&q->empty, 0, 0);
	sem_init(&q->full, 0, QUEUE_MAX);//Configuration::getInstance()->getOpticalLayerParameters().frameBufferSize
}

int array_spinlock_queue_push(SpinlockQueue *q, void *data)
{
	nanotimer_rt_start(&s);

	sem_wait(&q->full);

	pthread_spin_lock(&q->spin);
	/*
	if (q->count >= QUEUE_MAX) {
		pthread_spin_unlock(&q->spin);

		sem_wait(&q->full);

		pthread_spin_lock(&q->spin);
	}
	*/

	q->tail->data = data;
	
	if (q->tail == &(q->a[QUEUE_MAX-1])) {
		q->tail = &(q->a[0]);
	}
	else {
		q->tail++;
	}
	
	q->count++;

//	if (q->count == 1) {
	sem_post(&q->empty);
	//printf("Unlocked empty semaphore, count = %d\n", q->count);
//	}

	pthread_spin_unlock(&q->spin);

	nanotimer_rt_stop(&t);
	FILE *logfp = fopen("/tmp/queue-push.txt", "a+");
	fprintf(logfp, "%.2f\n", nanotimer_rt_ns_diff(&s, &t) / 1000.0);
	fclose(logfp);

	return 0;
}

int array_spinlock_queue_pull(SpinlockQueue *q, void **data_dest)
{
	nanotimer_rt_start(&s);

	sem_wait(&q->empty);

	pthread_spin_lock(&q->spin);

/*
	if (!q->count) {
		pthread_spin_unlock(&q->spin);

		printf("waiting sem empty...\n");
		sem_wait(&q->empty);
		printf("sem empty gained, count = %d\n", q->count);

		//return -1
		pthread_spin_lock(&q->spin);
	}
*/

	//printf("Assigning (q->head->data = %p to *data_dest (%p)\n", q->head->data, *data_dest);
	*data_dest = q->head->data;

	if (q->head == &(q->a[QUEUE_MAX-1])) {
		q->head = &(q->a[0]);
	}
	else {
		q->head++;
	}

	q->count--;
//	printf("PULL: data pull ok, count = %d\n", q->count);

//	if (q->count == (QUEUE_MAX - 1)) {
	sem_post(&q->full);
//	}

//	printf("unlocking and leaving\n");
	pthread_spin_unlock(&q->spin);
	
	nanotimer_rt_stop(&t);
	FILE *logfp = fopen("/tmp/queue-pull.txt", "a+");
	fprintf(logfp, "%.2f\n", nanotimer_rt_ns_diff(&s, &t) / 1000.0);
	fclose(logfp);

	return 0;
}

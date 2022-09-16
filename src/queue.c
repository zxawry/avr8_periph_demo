// file: queue.c
// author: awry

#include "queue.h"

// initiate the queue.
void queue_init(queue_t * queue)
{
	queue->tail = QUEUE_SIZE - 1;
	queue->head = 0;
	queue->size = 0;

	// this can be omitted if every queue is initiated only once
	uint8_t i = 0;
	while (i < QUEUE_SIZE)
		queue->data[i++] = '\0';
}

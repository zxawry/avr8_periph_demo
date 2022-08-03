// file: queue.c
// author: awry

#include "queue.h"

// initiate the queue.
void queue_init(queue_t * queue)
{
	queue->tail = QUEUE_SIZE - 1;
	queue->head = 0;
	queue->size = 0;

	// maybe not needed, since the compiler already initiates bss
	uint8_t i = 0;
	while (i < QUEUE_SIZE)
		queue->data[i++] = '\0';
}

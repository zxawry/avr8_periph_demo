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

// check if the queue is empty.
char queue_is_empty(queue_t * queue)
{
	return (char)(queue->size == 0);
}

// check if the queue is full.
char queue_is_full(queue_t * queue)
{
	return (char)(queue->size == QUEUE_SIZE);
}

// append an item to the tail of the queue.
void queue_enqueue(queue_t * queue, const char item)
{
	queue->tail = (queue->tail + 1) % QUEUE_SIZE;
	queue->data[queue->tail] = item;
	queue->size++;
}

// remove an item from the head of the queue.
char queue_dequeue(queue_t * queue)
{
	char item = queue->data[queue->head];
	queue->head = (queue->head + 1) % QUEUE_SIZE;
	queue->size--;
	return item;
}

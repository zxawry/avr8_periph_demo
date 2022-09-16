// file: queue.c
// author: awry

#include "queue.h"

#define ADVANCE_INDEX(i) ((i + 1) & QUEUE_SIZE_MASK)
//#define ADVANCE_INDEX(i) ((i + 1) % QUEUE_SIZE)

// initiate the queue.
void queue_init(queue_t * queue)
{
	queue->tail = 0;
	queue->head = 0;
	queue->size = 0;

	// this can be omitted if every queue is initiated only once
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
	queue->data[queue->tail] = item;
	queue->tail = ADVANCE_INDEX(queue->tail);
	queue->size++;
}

// remove an item from the head of the queue.
char queue_dequeue(queue_t * queue)
{
	char item = queue->data[queue->head];
	queue->head = ADVANCE_INDEX(queue->head);
	queue->size--;
	return item;
}

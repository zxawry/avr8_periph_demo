// file: queue.h
// author: awry

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdint.h>

// configure size of the queue.
#define QUEUE_SIZE (64)

// assume queue size is a power of two.
#define QUEUE_SIZE_MASK (QUEUE_SIZE - 1)

// advance indeces uniquely.
#define ADVANCE_INDEX(i) ((i + 1) & QUEUE_SIZE_MASK)

// queue type definition
typedef struct {
	uint8_t tail;
	uint8_t head;
	uint8_t size;
	char data[QUEUE_SIZE];
} queue_t;

// initiate the queue.
void queue_init(queue_t * queue);

// check if the queue is empty.
__attribute__((always_inline))
inline char queue_is_empty(queue_t * queue);

// check if the queue is full.
__attribute__((always_inline))
inline char queue_is_full(queue_t * queue);

// append an item to the tail of the queue.
__attribute__((always_inline))
inline void queue_enqueue(queue_t * queue, const char item);

// remove an item from the head of the queue.
__attribute__((always_inline))
inline char queue_dequeue(queue_t * queue);

// check if the queue is empty.
inline char queue_is_empty(queue_t * queue)
{
	return (char)(queue->size == 0);
}

// check if the queue is full.
inline char queue_is_full(queue_t * queue)
{
	return (char)(queue->size == QUEUE_SIZE);
}

// append an item to the tail of the queue.
inline void queue_enqueue(queue_t * queue, const char item)
{
	queue->data[queue->tail] = item;
	queue->tail = ADVANCE_INDEX(queue->tail);
	queue->size++;
}

// remove an item from the head of the queue.
inline char queue_dequeue(queue_t * queue)
{
	char item = queue->data[queue->head];
	queue->head = ADVANCE_INDEX(queue->head);
	queue->size--;
	return item;
}

#endif				// QUEUE_H_

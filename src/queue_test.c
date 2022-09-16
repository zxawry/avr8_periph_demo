// file: queue_test.c
// author: awry

#define __ASSERT_USE_STDERR

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#include "queue_test.h"
#include "queue.h"

// utility functions
int checked_queue_enqueue(queue_t * queue, char data);
int checked_queue_dequeue(queue_t * queue, char * data);
void print_queue_status(queue_t * queue);

int checked_queue_enqueue(queue_t * queue, const char data)
{
	if (queue_is_full(queue))
		return -1;
	queue_enqueue(queue, data);
	return 0;
}

int checked_queue_dequeue(queue_t * queue, char * data)
{
	if (queue_is_empty(queue))
		return -1;
	*data = queue_dequeue(queue);
	return 0;
}

void print_queue_status(queue_t * queue)
{
	fprintf(stderr, "head: %u, tail: %u, size: %u\n", \
		queue->head, queue->tail, queue->size);
}

// internal assertion functions
void assert_queue_init(queue_t * queue);
void assert_queue_enqueue(queue_t * queue, char item);
char assert_queue_dequeue(queue_t * queue);

void assert_queue_init(queue_t * queue)
{
	queue_init(queue);

	uint8_t i;
	for(i = 0; i < QUEUE_SIZE; i++)
		assert(queue->data[i] == 0);

	assert(queue_is_empty(queue) == true);
	assert(queue_is_full(queue) == false);
}

void assert_queue_enqueue(queue_t * queue, char item)
{
	uint8_t head = queue->head;
	uint8_t tail = queue->tail;
	uint8_t size = queue->size;

	queue_enqueue(queue, item);

	assert(queue->head == head);
	assert(queue->tail == ((tail + 1) & (QUEUE_SIZE - 1)));
	assert(queue->size == (size + 1));
	assert(queue->data[(tail + 1) & (QUEUE_SIZE - 1)] == item);
}

char assert_queue_dequeue(queue_t * queue)
{
	uint8_t head = queue->head;
	uint8_t tail = queue->tail;
	uint8_t size = queue->size;

	char item = queue_dequeue(queue);

	assert(queue->head == ((head + 1) & (QUEUE_SIZE - 1)));
	assert(queue->tail == tail);
	assert(queue->size == (size - 1));
	assert(queue->data[head] == item);

	return item;
}

// external assertion functions
void assert_queue_flush(queue_t * queue);
void assert_queue_ping_pong(queue_t * queue);
void assert_queue_pump_dump(queue_t * queue);

void assert_queue_flush(queue_t * queue)
{
	uint8_t i;

	for(i = queue->size; i > 0; i--)
		(void) queue_dequeue(queue);

	assert(queue_is_empty(queue) == true);
}

void assert_queue_ping_pong(queue_t * queue)
{
	assert_queue_flush(queue);

	char ping = 72;
	queue_enqueue(queue, ping);

	char pong;
	pong = queue_dequeue(queue);

	assert(ping == pong);
}

void assert_queue_pump_dump(queue_t * queue)
{
	assert_queue_flush(queue);

	uint8_t size = queue->size;

	const uint8_t free = QUEUE_SIZE - queue->size;

	char pump;
	for(pump = free; pump > 0; pump--) {
		assert(queue_is_full(queue) == false);
		queue_enqueue(queue, pump);
	}
	assert(queue_is_full(queue) == true);

	char dump;
	for(dump = free; dump > 0; dump--) {
		assert(queue_is_empty(queue) == false);
		assert(queue_dequeue(queue) == dump);
	}
	assert(queue->size == size);
}

int queue_test_main(void)
{
	queue_t queue;

	assert_queue_init(&queue);

	assert_queue_ping_pong(&queue);

	assert_queue_pump_dump(&queue);

	const char dummy[64] = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890<>";

	for(uint8_t i = 0; i < 64; i++)
		queue_enqueue(&queue, dummy[i]);

	assert(queue_is_full(&queue) == true);

	for(uint8_t i = 0; i < 64; i++)
		assert(queue_dequeue(&queue) == dummy[i]);

	assert(queue_is_empty(&queue) == true);

	assert_queue_ping_pong(&queue);

	assert_queue_pump_dump(&queue);

	assert_queue_flush(&queue);

	char given[16] = "0123456789abcdef";
	char taken[16] = "xxxxxxxxxxxxxxxx";

	for(uint8_t i = 0; i < 16; i++)
		queue_enqueue(&queue, given[i]);

	for(uint8_t i = 0; i < 16; i++) {
		taken[i] = queue_dequeue(&queue);
		queue_enqueue(&queue, taken[i]);
	}

	for(uint8_t i = 0; i < 16; i++)
		assert(queue_dequeue(&queue) == given[i]);

	return 0;
}

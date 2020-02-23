#include <string.h>
#include "ring_buffer.h"

void init_ringbuffer(struct ring_buffer *rb)
{
	memset((char*)(rb->buffer), 0, 64);
	rb->head = 0;
	rb->tail = 0;
}

bool ringbuffer_is_full(struct ring_buffer *rb)
{
	return rb->tail == ((rb->head + 1) % 64);
}

bool ringbuffer_is_empty(struct ring_buffer *rb)
{
	return rb->tail == rb->head;
}

void ringbuffer_put(struct ring_buffer *rb, unsigned char ch)
{
	if (ringbuffer_is_full(rb))
		return;

	rb->buffer[rb->head] = ch;
	rb->head = (rb->head + 1) % 64;
}

unsigned char ringbuffer_get(struct ring_buffer *rb)
{
	if (ringbuffer_is_empty(rb))
		return 0;

	unsigned char ret = rb->buffer[rb->tail];
	rb->tail = (rb->tail + 1) % 64;
	return ret;
}


#include <global.h>
#include "indirect_ring_buffer.h"

IndirectRingBuffer::IndirectRingBuffer()
{
	this->head = 0;
	this->tail = 0;

	for (int i=0; i<indirect_ring_buffer::LEN; i++)
		this->buf[i] = NULL;
}

IndirectRingBuffer::IndirectRingBuffer(char* buf_[])
{
	initialize(buf_);
}

void IndirectRingBuffer::initialize(char *buf_[])
{
	this->head = 0;
	this->tail = 0;

	for (int i=0; i<indirect_ring_buffer::LEN; i++)
		this->buf[i] = buf_[i];
}

bool IndirectRingBuffer::is_full()
{
	return this->tail == ((this->head + 1) % indirect_ring_buffer::LEN);
}

bool IndirectRingBuffer::is_empty()
{
	return this->tail == this->head;
}

/* Get a free buffer, then put something to it. The putter calls it. */
char* IndirectRingBuffer::get_first_free_buffer()
{
	if (is_full())
		return NULL;

	char *ret = this->buf[this->head];
	this->head = (this->head + 1) % indirect_ring_buffer::LEN;

	return ret;
}

/* The getter(consumer) calls it. */
char* IndirectRingBuffer::get_first_used_buffer()
{
	if (is_empty())
		return NULL;

	char *ret = this->buf[this->tail];
	this->tail = (this->tail + 1) % indirect_ring_buffer::LEN;

	return ret;
}


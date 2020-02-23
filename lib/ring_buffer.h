#ifndef __LIB_RING_BUFFER_H__
#define __LIB_RING_BUFFER_H__

struct ring_buffer {
	unsigned char buffer[64];
	unsigned char head;
	unsigned char tail;
};

void init_ringbuffer(struct ring_buffer *rb);
bool ringbuffer_is_full(struct ring_buffer *rb);
bool ringbuffer_is_empty(struct ring_buffer *rb);
void ringbuffer_put(struct ring_buffer *rb, unsigned char ch);
unsigned char ringbuffer_get(struct ring_buffer *rb);

#endif


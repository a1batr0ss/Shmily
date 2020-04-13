#ifndef __LIB_INDIRECT_RING_BUFFER_H__
#define __LIB_INDIRECT_RING_BUFFER_H__

class IndirectRingBuffer {
  private:
	char* buf[10];
	unsigned char head;
	unsigned char tail;

  public:
	IndirectRingBuffer();
	IndirectRingBuffer(char* buf_[]);
	void initialize(char* buf_[]);
	bool is_full();
	bool is_empty();
	char* get_first_free_buffer();
	char* get_first_used_buffer();
};

namespace indirect_ring_buffer {
	const unsigned char LEN = 10;
};

#endif


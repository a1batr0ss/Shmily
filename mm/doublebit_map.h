#ifndef __MM_DOUBLEBIT_MAP_H__
#define __MM_DOUBLEBIT_MAP_H__

class DoublebitMap {
  private:
	unsigned int bytes_len;
	unsigned char *base;

	unsigned char get_slot_status(unsigned int idx);
	void sync(unsigned int start, unsigned int cnt);

  public:
	DoublebitMap();
	void initialize(unsigned int bytes_len, unsigned char *base_);
	void set_slot_value(unsigned int idx, unsigned char value);
	int get_free_slots(unsigned int cnt);
	unsigned int free_slots(unsigned int idx);
};

namespace {
	namespace doublebmap {
		unsigned char SLOT_FREE = 0x0;  /* 00 */
		unsigned char SLOT_USED_SINGLE = 0x1;  /* 01 */
		unsigned char SLOT_SEQ_START = 0x2;  /* 10 */
		unsigned char SLOT_SEQ_END = 0x3;  /* 11 */
	};
};

#endif


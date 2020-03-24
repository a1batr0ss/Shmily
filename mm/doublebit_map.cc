#include <global.h>
#include <stdio.h>
#include <string.h>
#include "doublebit_map.h"

DoublebitMap::DoublebitMap() : bytes_len(0), base(NULL) {}

void DoublebitMap::initialize(unsigned int bytes_len_, unsigned char *base_)
{
	this->bytes_len = bytes_len_;
	this->base = base_;

	memset((char*)(this->base), this->bytes_len, 0);
}

unsigned char DoublebitMap::get_slot_status(unsigned int idx)
{
	idx *= 2;
	unsigned byte_idx = idx / 8;
	unsigned bit_idx = (idx % 8);
	return (this->base[byte_idx] & (0x3 << bit_idx)) >> bit_idx;
}

void DoublebitMap::set_slot_value(unsigned int idx, unsigned char val)
{
	idx *= 2;
	unsigned int byte_idx = idx / 8;
	unsigned int bit_idx = (idx % 8);

	// this->base[byte_idx] &= (0 << bit_idx);  /* All set to zero. */
	this->base[byte_idx] &= ~(3 << bit_idx);
	this->base[byte_idx] |= val << bit_idx;
}

void DoublebitMap::sync(unsigned int start, unsigned int cnt)
{
	if (1 == cnt) {
		set_slot_value(start, doublebmap::SLOT_USED_SINGLE);
		return;
	}

	// printf("start %x %x\n", start, start+cnt-1);
	set_slot_value(start, doublebmap::SLOT_SEQ_START);
	for (int i=1; i<cnt-1; i++)
		set_slot_value(start+i, doublebmap::SLOT_USED_SINGLE);

	set_slot_value(start+cnt-1, doublebmap::SLOT_SEQ_END);
}

int DoublebitMap::get_free_slots(unsigned int cnt)
{
	unsigned int found_cnt = 0;
	for (int idx=0; idx<(this->bytes_len * 8); idx++) {
		if ((doublebmap::SLOT_FREE == get_slot_status(idx)) && (1 == cnt)) {
			sync(idx, cnt);
			return idx;
		} else if (doublebmap::SLOT_FREE == get_slot_status(idx)) {
			found_cnt++;

			if (found_cnt == cnt) {

				sync(idx - cnt + 1, cnt);
				return (idx - cnt + 1);
			}
		} else {
			found_cnt = 0;
		}
	}

	return -1;
}

unsigned int DoublebitMap::free_slots(unsigned int idx)
{
	unsigned int cnt = 0;
	if (doublebmap::SLOT_USED_SINGLE == get_slot_status(idx)) {
		set_slot_value(idx, doublebmap::SLOT_FREE);
		return 1;
	}

	if (doublebmap::SLOT_SEQ_START != get_slot_status(idx)) {
		printf("illegal!!! %x\n", get_slot_status(idx));
		return 0;
	}

	while (doublebmap::SLOT_SEQ_END != get_slot_status(idx)) {
		set_slot_value(idx++, doublebmap::SLOT_FREE);
		cnt++;
	}

	/* The END. */
	set_slot_value(idx, doublebmap::SLOT_FREE);

	return ++cnt;
}


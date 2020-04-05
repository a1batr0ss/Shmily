#ifndef __MM_SMALL_MEM_HEADER_H__
#define __MM_SMALL_MEM_HEADER_H__

#include <cycle_list.h>

struct small_mem_header {
	struct cyclelist_elem page_among_queue;
	unsigned int each_block_size;
	unsigned int free_cnts;
	CycleList page_in_free_queue;
};

void install_mem_header(void *page, unsigned int each_block_size);

#endif


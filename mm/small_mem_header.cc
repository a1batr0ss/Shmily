#include <global.h>
#include <stdio.h>
#include <cycle_list.h>
#include "small_mem_header.h"

void install_mem_header(void *page, unsigned int each_block_size)
{
	if (0 != (unsigned int)page % paging::page_size)
		return;

	struct small_mem_header *page_header = (struct small_mem_header*)page;
	page_header->each_block_size = each_block_size;
	page_header->free_cnts = (paging::page_size - sizeof(struct small_mem_header)) / each_block_size;
	page_header->page_in_free_queue.initialize();

	struct cyclelist_elem *elem = (struct cyclelist_elem*)(page + sizeof(small_mem_header));
	for (int i=0; i<page_header->free_cnts; i++) {
		// printf("elem %x %d\n", elem, page_header->free_cnts);
		page_header->page_in_free_queue.append(elem);

		elem = (struct cyclelist_elem*)((unsigned int)elem + page_header->each_block_size);
	}
}


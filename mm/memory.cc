#include <global.h>
#include <string.h>
#include <print.h>
#include <stdio.h>
#include <bitmap.h>
#include "memory.h"
#include "doublebit_map.h"
#include "small_mem_header.h"
#include "swar.h"

#define DIV_ROUND_UP(x, y) ((x + y - 1) / (y))

MemoryManager::MemoryManager()
{
	/* Initialize the memory pool. */
	unsigned int mem_capacity = get_mem_capacity();

    unsigned int stationary_mem = 0x200000;  /* more than 2M can't be accessed by other processes */
    unsigned int free_mem = mem_capacity - stationary_mem;
    unsigned int free_pages = free_mem / paging::page_size;

    unsigned int bmap_length = free_pages / 8;
    unsigned int phy_start = 0x200000;

    this->phypool.phyaddr_start = phy_start;
    this->phypool.pool_size = free_mem;
    this->phypool.phyaddr_bmap.bytes_len = bmap_length;
    this->phypool.phyaddr_bmap.base = (unsigned char*)0x9000;

    init_bitmap(&(this->phypool.phyaddr_bmap));

    /* Initialise the virtaul address bitmap. */
    this->initproc_vir.vaddr_start = 0x200000;

    // init_bitmap(&(this->initproc_vir.vaddr_bmap));
	this->initproc_vir.vaddr_bmap.initialize(bmap_length, (unsigned char*)(0x9000 + bmap_length));
}

unsigned int MemoryManager::get_mem_capacity()
{
   return *(unsigned int*)0x548;
}

void* MemoryManager::get_vir_page(unsigned int cnt)
{
    int idx_start = -1;
    int vaddr_start = 0;
	idx_start = this->initproc_vir.vaddr_bmap.get_free_slots(cnt);

    if (-1 == idx_start)
        return NULL;

    vaddr_start = this->initproc_vir.vaddr_start + idx_start*paging::page_size;

    return (void*)vaddr_start;
}

void* MemoryManager::get_phy_page()
{
    int idx_free = bitmap_scan(&(this->phypool.phyaddr_bmap), 1);

    if (-1 == idx_free)
        return NULL;

    bitmap_set_bit(&(this->phypool.phyaddr_bmap), idx_free, 1);
    unsigned int phyaddr = this->phypool.phyaddr_start + idx_free*paging::page_size;
    return (void*)phyaddr;
}

static unsigned int* vaddr2pde(unsigned int vaddr)
{
    unsigned int *pde = (unsigned int*)((0xfffff000) + \
            4 * ((vaddr&0xffc00000) >> 22));
    return pde;
}

static unsigned int* vaddr2pte(unsigned int vaddr)
{
    unsigned int *pte = (unsigned int*)(0xffc00000 + \
            ((vaddr&0xffc00000)>>10) + \
            4 * ((vaddr&0x003ff000)>>12));
}

static unsigned int vaddr2phy(unsigned int vaddr)
{
	unsigned int *pte = vaddr2pte(vaddr);
	return ((*pte & 0xfffff000) + (vaddr & 0x00000fff));
}

void MemoryManager::map_vir_phy(unsigned int viraddr, unsigned int phyaddr)
{
    unsigned int *pde_vaddr = vaddr2pde(viraddr);
    unsigned int *pte_vaddr = vaddr2pte(viraddr);
    unsigned int is_present = 0x00000001;

    if (*pde_vaddr & is_present) {
        if (*pte_vaddr & is_present) {
            /* Should not exists. */
        } else {
            *pte_vaddr = phyaddr | paging::US_U | paging::RW_W | paging::P;
        }
    } else {
        /* This pde is not exists, which means the page table is not exists. */
        unsigned int pg_table = (unsigned int)get_phy_page();
        *pde_vaddr = pg_table | paging::US_U | paging::RW_W | paging::P;
        memset((char*)((unsigned int)pte_vaddr&0xfffff000), 0, paging::page_size);
        *pte_vaddr = phyaddr | paging::US_U | paging::RW_W | paging::P;
    }
}

void* MemoryManager::malloc_page(unsigned int cnt)
{
    void *vaddr_start = get_vir_page(cnt);
    unsigned int vaddr = (unsigned int)vaddr_start;
    while (cnt-- > 0) {
        void *phyaddr_start = get_phy_page();

        map_vir_phy((unsigned int)vaddr, (unsigned int)phyaddr_start);
        vaddr += paging::page_size;
    }
    return vaddr_start;
}

void MemoryManager::free_phy_page(unsigned int phyaddr)
{
	unsigned int idx = (phyaddr - this->phypool.phyaddr_start ) / paging::page_size;
	bitmap_set_bit(&phypool.phyaddr_bmap, idx, 0);
}

unsigned int MemoryManager::free_vir_page(unsigned int viraddr)
{
	unsigned int cnt = 0;
	unsigned idx = (viraddr - this->initproc_vir.vaddr_start) / paging::page_size;

	cnt = this->initproc_vir.vaddr_bmap.free_slots(idx);
	return cnt;
}

void MemoryManager::remove_map_addr(unsigned int viraddr)
{
	unsigned int *pte = vaddr2pte(viraddr);
	*pte &= ~paging::P;

	/* INVLPG: privilege instruction, if run below protect mode, the CPL must be zero. Now our MM is a user process.(CPL is 3) So we can't execute it. */
	// asm volatile ("invlpg %0" :: "m" (viraddr) : "memory");
}

void MemoryManager::free_page(void *viraddr)
{
	unsigned int vaddr = (unsigned int)viraddr;
	unsigned int paddr = 0;
	unsigned int cnt_ = free_vir_page((unsigned int)viraddr);

	while ((cnt_--) > 0) {
		paddr = vaddr2phy(vaddr);
		free_phy_page(paddr);
		remove_map_addr(vaddr);

		vaddr += paging::page_size;
	}
}

void* MemoryManager::malloc(unsigned int cnt_bytes)
{
	if (0 == cnt_bytes)
		return NULL;

	if (cnt_bytes > 1024) {
		unsigned int page_cnt = DIV_ROUND_UP(cnt_bytes, paging::page_size);
		void *ret = malloc_page(page_cnt);

		memset((char*)ret, 0, page_cnt*paging::page_size);
		return ret;
	} else {
		unsigned int desc_idx = 0;
		/* Get the descriptor. */
		for (unsigned int cnt_start=16; cnt_start<cnt_bytes; cnt_start*=2, desc_idx++) ;

		int l = mem_desc[desc_idx].get_length();
		void *ret = NULL;
		for (int i=0; i<l; i++) {
			struct cyclelist_elem *elem = mem_desc[desc_idx].get_elem(i);
			struct small_mem_header *page_header = (struct small_mem_header*)elem;

			if (0 == page_header->free_cnts)
				continue;

			/* If this page has free slot, return it. */
			ret = page_header->page_in_free_queue.pop();
			page_header->free_cnts--;

			/* If this page is full, put it to tail. So we need not scan it every time. */
			if (0 == page_header->free_cnts) {
				mem_desc[desc_idx].remove(elem);
				mem_desc[desc_idx].append(elem);
			}

			memset((char*)ret, 0, page_header->each_block_size);

			return ret;
		}

		/* All pages we hold are full. Then allocate a page. */
		void *new_page = malloc_page(1);
		install_mem_header(new_page, 2<<(desc_idx+3));
		mem_desc[desc_idx].push((struct cyclelist_elem*)new_page);  /* Append the page to queue. */

		struct small_mem_header *page_header = (struct small_mem_header*)new_page;

		ret = page_header->page_in_free_queue.pop();
		page_header->free_cnts--;

		memset((char*)ret, 0, page_header->each_block_size);

		return ret;
	}
}

void MemoryManager::free(void *buf)
{
	unsigned int buf_uint = (unsigned int)buf;

	/* This condition is trustless. If the buf is small memory block, as the page_header, it couldn't exactly divided by page size. */
	if (0 == buf_uint % paging::page_size) {
		free_page(buf);
	} else {
		struct small_mem_header *page_header = (struct small_mem_header*)((unsigned int)buf & 0xfffff000);
		page_header->page_in_free_queue.append((struct cyclelist_elem*)buf);
		(page_header->free_cnts)++;

		/* If the page's free block occupy 1/5, then put it to head, so we can get is when we need malloc. */
		if ((5 * page_header->free_cnts) >= ((paging::page_size - sizeof(struct small_mem_header) / page_header->each_block_size))) {

			unsigned int desc_idx = 0;
			for (unsigned int cnt_start=16; cnt_start<page_header->each_block_size; cnt_start*=2, desc_idx++) ;

			mem_desc[desc_idx].remove((struct cyclelist_elem*)page_header);
			mem_desc[desc_idx].push((struct cyclelist_elem*)page_header);
		}
	}
}

/* Roughly calculate the memory information. */
void MemoryManager::print_mem_info()
{
	unsigned int all_mem = get_mem_capacity();
    unsigned int *start = (unsigned int*)(this->phypool.phyaddr_bmap.base);

	unsigned int used_pages = 0;
	for (unsigned int i=0; i<(this->phypool.phyaddr_bmap.bytes_len / 4); i++)
		used_pages += swar(start[i]);

	unsigned int all_used = used_pages*paging::page_size;
	printf("totoal: %dKB   used: %dKB   free:  %dKB\n", all_mem>>10, all_used>>10, (all_mem-all_used)>>10);
}


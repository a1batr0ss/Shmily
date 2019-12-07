#include <global.h>
#include "page.h"
#include "string.h"

void install_page();
void set_cr3(unsigned int pagedir_base);
void enable_PG();

void enable_paging()
{
    install_page();
}

void install_page()
{
    /* set page directory table and page table */
    unsigned int *pagedir_base = (unsigned int*)paging::page_dir_base;
    unsigned int *first_pagetable = (unsigned int*)((unsigned int)pagedir_base + paging::page_size);
    unsigned int default_pageattr = paging::US_U | paging::RW_W | paging::P;

    /* set page directory table */
    memset((char*)pagedir_base, 0, paging::page_size);
    *pagedir_base = (unsigned int)first_pagetable | default_pageattr;

    /* the last one point to start, so that we can use virtual address to access page directory table. */
    *(pagedir_base + 1023) = (unsigned int)pagedir_base + default_pageattr;
    
    /* set the first page table */
    unsigned int entry = 0;
    for (int idx=0; idx<528; idx++) {
        first_pagetable[idx] = entry | default_pageattr;
        entry += 4096;
    }

    /* set cr3 */
    set_cr3((unsigned int)pagedir_base);

    /* enable PG */
    enable_PG();
}

void set_cr3(unsigned int pagedir_base)
{
    asm volatile ("movl %0, %%cr3":: "r" (pagedir_base) :"memory");
}

void enable_PG()
{
    asm volatile ("movl %%cr0, %%eax \n\t"
                    "orl $0x80000000, %%eax \n\t"
                    "movl %%eax, %%cr0":::);
}


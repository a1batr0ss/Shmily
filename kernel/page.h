#ifndef __KERNEL_PAGE_H__
#define __KERNEL_PAGE_H__

struct pte {
    unsigned int present    :1;
    unsigned int rw         :1;
    unsigned int us         :1;
    unsigned int pwt        :1;
    unsigned int pcd        :1;
    unsigned int accessed   :1;
    unsigned int dirty      :1;
    unsigned int pat_or_ps  :1;
    unsigned int global     :1;
    unsigned int available  :3;
    unsigned int pagebase   :20;
} __attribute__((packed));

struct page_table {
    struct pte pages[1024];
};

void enable_paging();

#endif


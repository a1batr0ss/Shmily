#ifndef __TOOLS_ELF_H__
#define __TOOLS_ELF_H__

struct elf32_ehdr {
	unsigned char ident[16];
	unsigned short type;
	unsigned short machine;
	unsigned int version;
	unsigned int entry;
	unsigned int phoff;
	unsigned int shoff;
	unsigned int flags;
	unsigned short ehsize;
	unsigned short phentsize;
	unsigned short phnum;
	unsigned short shentsize;
	unsigned short shnum;
	unsigned short shstrndx;
};

struct elf32_phdr {
	unsigned int type;
	unsigned int offset;
	unsigned int vaddr;
	unsigned int paddr;
	unsigned int filesz;
	unsigned int memsz;
	unsigned int flags;
	unsigned int algin;
};

enum seg_type {
	PT_NULL, PT_LOAD, PT_DYNAMIC, PT_INTERP, PT_NOTE, PT_SHLIB, PT_PHDR
};

void analyse_elf(int in_file, int out_file);

#endif


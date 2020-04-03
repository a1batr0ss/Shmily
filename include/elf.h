#ifndef __INCLUDE_ELF_H__
#define __INCLUDE_ELF_H__

struct elf_header {
	unsigned char ident[16];
	unsigned short type;
	unsigned short machine;
	unsigned int version;
	unsigned int entry_point;
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

struct elf_program_header {
	unsigned int type;
	unsigned int offset;
	unsigned int vaddr;
	unsigned int paddr;
	unsigned int file_size;
	unsigned int mem_size;
	unsigned int flags;
	unsigned int align;
};

namespace elf {
	const unsigned char SEG_NULL = 0;
};

#endif


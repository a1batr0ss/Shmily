#include <unistd.h>
#include <iostream>
#include <cstring>
#include "elf.h"

using std::cout;
using std::endl;

void analyse_elf(int in_file, int out_file)
{
	int n = 0;
	char buf[256] = {0};
	struct elf32_ehdr ehdr;
	struct elf32_phdr phdr;
	memset(&phdr, 0, sizeof(struct elf32_phdr));
	memset(&ehdr, 0, sizeof(struct elf32_ehdr));

	n = read(in_file, &ehdr, 256);

	cout << ehdr.ident << endl;
}



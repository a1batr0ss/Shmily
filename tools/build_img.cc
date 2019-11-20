#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <cstring>
// #include "elf.h"

char m_table[64] = {0};
unsigned int *p_mtable = (unsigned int*)m_table;

void install_module_table(int image, unsigned int offset)
{
	printf("offset is %d\n", offset);
	*p_mtable = 64 + offset;
	p_mtable++;
}

int main()
{
	char buf[256] = {0};
	int n = 0;
	const char *kernel_path = "../build/kernel";
	const char *mm_path = "../build/mm";
	const char *fs_path = "../build/fs";
	const char *dr_path = "../build/dr";
	const char *image_path = "../build/image";

	int image = open(image_path, O_CREAT | O_WRONLY, S_IRWXU);
	int kernel = open(kernel_path, O_RDONLY);
	int mm = open(mm_path, O_RDONLY);
	int fs = open(fs_path, O_RDONLY);
	int dr = open(dr_path, O_RDONLY);

	struct stat kernel_info;
	struct stat mm_info;
	struct stat fs_info;
	stat(kernel_path, &kernel_info);
	stat(mm_path, &mm_info);
	stat(fs_path, &fs_info);

	install_module_table(image, 0);
	install_module_table(image, 0 + kernel_info.st_size);
	install_module_table(image, 0 + kernel_info.st_size + mm_info.st_size);
	install_module_table(image, 0 + kernel_info.st_size + mm_info.st_size + fs_info.st_size);
	write(image, m_table, 64);

	while (n = read(kernel, buf, 256))
		write(image, buf, n);

	memset(buf, 0, 256);
	while (n = read(mm, buf, 256))
		write(image, buf, n);

	memset(buf, 0, 256);
	while (n = read(fs, buf, 256))
		write(image, buf, n);

	memset(buf, 0, 256);
	while (n = read(dr, buf, 256))
		write(image, buf, n);

	close(image);
	close(kernel);
	close(mm);
	close(fs);
	close(dr);

	return 0;
}


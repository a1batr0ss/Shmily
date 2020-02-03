#ifndef __FS_FS_H__
#define __FS_FS_H__

namespace {
	namespace _fs {
		unsigned int sector_size = 512;
		unsigned int max_file_nr = 1024;
		unsigned int min_file_nr = 64;
	};
}

void init_fs();

#endif


#ifndef __FS_FS_H__
#define __FS_FS_H__

namespace {
	namespace _fs {
		unsigned int sector_size = 512;
	};
}

void init_fs();

#endif


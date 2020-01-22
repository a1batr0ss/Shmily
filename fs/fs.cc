#include <global.h>
#include <stdio.h>
#include <string.h>
#include <all_syscall.h>
#include "fs.h"
#include "super_block.h"

enum partition_type {
	PRIMARY, EXTEND, LOGIC
};

struct partition {
	char name[16];
	unsigned int sector_cnt;
	unsigned int start_lba;
	enum partition_type type;
	struct disk *disk;
	struct super_block *sb;
};

struct disk {
	char name[16];
	struct channel *channel;
	bool is_slave;
	struct partition primary[4];
	struct partition logic[8];
};

struct channel {
	char name[16];
	unsigned int port;
	unsigned int irq_nr;
	struct disk disks[2];
};

void partition_install_fs(struct partition *part, unsigned int disk_nr);

/* Just for the slave disk. */
void init_fs()
{
	printf("init fs.\n");

	unsigned int disk_nr = 1;
	struct super_block *buf = (struct super_block*)malloc(_fs::sector_size);
	struct disk *disk = (struct disk*)get_disk(disk_nr);

	if (NULL == buf)
		return;

	/* Process primary partitions. */
	for (int i=0; i<4; i++) {
		struct partition *prim_part = &disk->primary[i];

		if (0 == prim_part->sector_cnt)
			continue;

		/* We recorded the extend partition, buf we needn't handle it(Just as offset). */
		if (EXTEND == prim_part->type)
			continue;
		
		memset((char*)buf, 0, _fs::sector_size);

		read_disk(disk_nr, prim_part->start_lba+1, (char*)buf, 1);

		if (!strcmp(buf->magic, "Shmily"))
			partition_install_fs(prim_part, disk_nr);
	}

	/* Process logic partitions. */
	for (int i=0; i<4; i++) {
		struct partition *logic_part = &disk->logic[i];

		if (0 == logic_part->sector_cnt)
			continue;

		memset((char*)buf, 0, _fs::sector_size);

		read_disk(disk_nr, logic_part->start_lba+1, (char*)buf, 1);

		if (!strcmp(buf->magic, "Shmily"))
			partition_install_fs(logic_part, disk_nr);
	}
	
	free(buf);
	printf("end init fs.\n");
}

void partition_install_fs(struct partition *part, unsigned int disk_nr)
{
	struct super_block *sb = (struct super_block*)malloc(_fs::sector_size);
	printf("%s is installing file system.\n", part->name);
	memset((char*)sb, 0, _fs::sector_size);

	strcpy(sb->magic, "Shmily");
	sb->sectors_nr = part->sector_cnt;
	sb->start_lba = part->start_lba;

	write_disk(disk_nr, part->start_lba+1, (char*)sb, 1);
			
	free(sb);
}


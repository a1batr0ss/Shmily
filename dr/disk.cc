#include <global.h>
#include <ipc_glo.h>
#include <io.h>
#include <string.h>
#include <syscall.h>
#include <stdio.h>
#include <all_syscall.h>
#include "disk.h"

struct disk disks[2];

void disk_handler();

void traverse_disk_partition(unsigned int disk_nr);
void print_disk_partition_info(unsigned int disk_nr);

void init_disk()
{
	unsigned char disk_nr = *(unsigned char*)0x475;

	/* TODO: Initializing the disks. Now we just have two disks. */
	disks[0].is_slave = false;
	disks[1].is_slave = true;

	register_intr_handler(0x2e, (void*)disk_handler);	
	register_intr_handler(0x2f, (void*)disk_handler);	

	/* We just handle the slave disk.(fs_test.img) */
	traverse_disk_partition(1);
}

/* Just sleep for a moment. */
static void sleep_seconds_demo(unsigned int sec)
{
	int i, j, k;
	for (i=0; i<sec; i++)
		for (j=0; j<1000; j++)
			for (k=0; k<1000; k++) ;
}

void disk_handler()
{
	inb(hd::status_reg);  /* Pretend this interrupt is handled. */

	Message msg(all_processes::INTERRUPT);
	msg.send(all_processes::DR);
}

static void choose_disk(struct disk *disk)
{
	unsigned char reg_con = 0;
	if (disk->is_slave)
		reg_con = hd::dev_mbs | hd::dev_lba | hd::dev_dev;
	else
		reg_con = hd::dev_mbs | hd::dev_lba;
	
	outb(hd::dev_reg, reg_con);
}

static void choose_sector(struct disk *disk, unsigned int lba, unsigned int cnt)
{
	outb(hd::sect_cnt_reg, cnt);
	outb(hd::lba_l_reg, lba);
	outb(hd::lba_m_reg, lba >> 8);
	outb(hd::lba_h_reg, lba >> 16);

	unsigned char reg_con = hd::dev_mbs | hd::dev_lba | (lba>>24);
	if (disk->is_slave)
		reg_con |= hd::dev_dev;

	outb(hd::dev_reg, reg_con);
}

static bool wait_disk()
{
	Message msg(all_processes::DR);
	struct _context con;
	con.con_1 = 3;
	unsigned max_wait_seconds = 30;

	while (max_wait_seconds-=1 > 0) {
		if (inb(hd::status_reg & hd::stat_busy))
			return inb(hd::status_reg) & hd::stat_ready;
		else
			msg.send_then_recv(all_processes::KR);
	}

	return false;
}

static void swap_pairs_bytes(const char *dst, char *buf, unsigned int len)
{
	unsigned char idx;
	for (idx=0; idx<len; idx+=2) {
		buf[idx+1] = *dst++;
		buf[idx] = *dst++;
	}
	buf[idx] = '\0';
}

static void read_disk(char *buf, unsigned char cnt)
{
	unsigned int bytes_cnt = cnt * 512;
	if (0 == cnt)
		bytes_cnt = 256 * 512;

	insw(hd::data_reg, buf, bytes_cnt/2);
}

static void write_disk(char *buf, unsigned char cnt)
{
	unsigned int bytes_cnt = cnt * 512;
	if (0 == cnt)
		bytes_cnt = 256 * 512;

	outsw(hd::data_reg, buf, bytes_cnt/2);
}

void write_sector(struct disk *disk, unsigned int lba, char *buf, unsigned int cnt);

void disk_identify(unsigned char disk_nr)
{
	choose_disk(&disks[disk_nr]);

	outb(hd::cmd_reg, hd::identify);

	/* TODO: transmit information with interrupt. Don't have a appropriate method. */
	Message msg(all_processes::DR);
	msg.receive(all_processes::INTERRUPT);

	if (!wait_disk()) {
		printf("Not ready.\n");
		return;
	}

	char info_buf[512];
	read_disk(info_buf, 1);

	char buf[64];
	memset(buf, 0, 64);
	swap_pairs_bytes(&info_buf[20], buf, 20);	
	printf("SN is %s\n", buf);

	// write_disk("Halou World", 1);  // Couldn't write to disk directly.	
	// write_sector(&disks[1], 0, "Halou World", 1);
}

/* TODO: Taking measures to guarantee concurrect security. */
static void _read_sector(struct disk *disk, unsigned int lba, char *buf, unsigned int cnt)
{
	choose_disk(disk);
	choose_sector(disk, lba, cnt);
	outb(hd::cmd_reg, hd::read);

	Message msg(all_processes::DR);
	msg.receive(all_processes::INTERRUPT);

	if (!wait_disk()) {
		printf("Read from sector failed.\n");
		return;
	}

	read_disk(buf, cnt);
}

/* TODO: Taking measures to guarantee concurrect security. */
static void _write_sector(struct disk *disk, unsigned int lba, char *buf, unsigned int cnt)
{
	choose_disk(disk);
	choose_sector(disk, lba, cnt);
	outb(hd::cmd_reg, hd::write);

	Message msg(all_processes::DR);
	msg.receive(all_processes::INTERRUPT);

	if (!wait_disk()) {
		printf("Write to sector failed.\n");
		return;
	};

	write_disk(buf, cnt);
}

void read_sector(unsigned int disk_nr, unsigned int lba, char *buf, unsigned int cnt)
{
	_read_sector(&disks[disk_nr], lba, buf, cnt);
}

void write_sector(unsigned int disk_nr, unsigned int lba, char *buf, unsigned int cnt)
{
	_write_sector(&disks[disk_nr], lba, buf, cnt);
}

void get_partition_table(unsigned int disk_nr, unsigned int lba, struct partition_table_mbr *p_tbl)
{
	char *buf = (char*)malloc(512);

	read_sector(disk_nr, lba, buf, 1);
	memcpy((char*)p_tbl, buf+0x1be, 64);	

	free(buf);	
}


void print_disk_partition_info(unsigned int disk_nr)
{
	struct disk *disk = &disks[disk_nr];

	for (int i=0; i<4; i++) {
		struct partition *part = &(disk->primary[i]);
		
		if (0 != part->sector_cnt)
			printf("%s, start at %d, size is %dMB.\n", part->name, part->start_lba,(part->sector_cnt * 512) / (1024*1024));
	}
	
	printf("logic:\n");
	for (int i=0; i<4; i++) {
		struct partition *part = &(disk->logic[i]);
		
		if (0 != part->sector_cnt)
			printf("%s, start at %d, size is %dMB.\n", part->name, part->start_lba,(part->sector_cnt * 512) / (1024*1024));

	}
}

void process_disk_ebr(unsigned int disk_nr, struct partition_table_entry_mbr *entry);

void traverse_disk_partition(unsigned int disk_nr)
{
	struct disk *disk = &disks[disk_nr];
	struct partition_table_mbr mbr_tbl;

	memset((char*)&mbr_tbl, 0, sizeof(struct partition_table_mbr));

	get_partition_table(disk_nr, 0, &mbr_tbl);
	
	/* Process primary partition. */
	for (int i=0; i<4; i++) {
		if (0 != mbr_tbl.entries[i].sector_nr_in) {  /* The entry is valid. */
			sprintf(disk->primary[i].name, "sda%d", i);
			disk->primary[i].start_lba = mbr_tbl.entries[i].sector_nr_before;
			disk->primary[i].sector_cnt = mbr_tbl.entries[i].sector_nr_in;

			/* Process extend partition. */
			if (0x5 == mbr_tbl.entries[i].sys_signature)
				process_disk_ebr(disk_nr, &(mbr_tbl.entries[i]));
		}
	}
}


/* Get the information about extend partition. */
void process_disk_ebr(unsigned int disk_nr, struct partition_table_entry_mbr *entry)
{
	unsigned int logic_idx = 0;
	unsigned int extend_offset = entry->sector_nr_before;
	struct disk *disk = &disks[disk_nr];

	struct partition_table_mbr ebr_tbl;
	get_partition_table(disk_nr, entry->sector_nr_before, &ebr_tbl);

	while (0 != ebr_tbl.entries[0].sector_nr_in) {
		sprintf(disk->logic[logic_idx].name, "sda%d", logic_idx+5);
		disk->logic[logic_idx].start_lba = ebr_tbl.entries[0].sector_nr_before + extend_offset;
		disk->logic[logic_idx].sector_cnt = ebr_tbl.entries[0].sector_nr_in;

		logic_idx++;

		unsigned int ebr_offset = ebr_tbl.entries[1].sector_nr_before + extend_offset;

		if (0 != ebr_tbl.entries[1].sector_nr_in)
			get_partition_table(disk_nr, ebr_offset, &ebr_tbl);
		else
			break;
		
		extend_offset = ebr_offset;
	}
}


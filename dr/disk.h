#ifndef __INIT_DISK_H__
#define __INIT_DISK_H__

namespace disk {
    const unsigned int disk_intr_nr = 0x2e;
    const unsigned short channel_port_base = 0x1f0;
    const unsigned short data_reg = 0x1f0;
    const unsigned short error_reg = 0x1f1;
    const unsigned short sect_cnt_reg = 0x1f2;
    const unsigned short lba_l_reg = 0x1f3;
    const unsigned short lba_m_reg = 0x1f4;
    const unsigned short lba_h_reg = 0x1f5;
    const unsigned short dev_reg = 0x1f6;
    const unsigned short status_reg = 0x1f7;
    const unsigned short cmd_reg = 0x1f7;

    const unsigned char identify = 0xec;
    const unsigned char read = 0x20;
    const unsigned char write = 0x30;

    const unsigned char busy = 0x80;
    const unsigned char unready = 0x40;
    const unsigned char ready = 0x08;
};

#endif


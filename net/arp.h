#ifndef __NET_ARP_H__
#define __NET_ARP_H__

struct arp_packet {
	unsigned short hw_type;
	unsigned short req_protocol;
	unsigned char hw_addr_size;
	unsigned char protocol_size;
	unsigned short opcode;
	unsigned char src_mac_addr[6];
    unsigned char src_ip_addr[4];
	unsigned char dst_mac_addr[6];
	unsigned char target_ip[4];
};

/* The item which is in arp cache table. */
struct mac_map_ip {
	unsigned char mac_addr[6];
	unsigned char ip_addr[4];
	unsigned int ref;  /* Decide to remove the items. */
};

class ArpFactory {
  private:
	static void format_request_packet(unsigned char *data, unsigned char *mac_addr, unsigned char *ip_addr, unsigned char *target_ip);
	static void format_reply_packet(unsigned char *data, unsigned char *mac_addr, unsigned char *ip_addr, unsigned char *res_mac_addr, unsigned char *req_ip);

  public:
	static void request(unsigned char *mac_addr, unsigned char *ip_addr, unsigned char *target_ip);
	static void reply(unsigned char *mac_addr, unsigned char *ip_addr, unsigned char *target_ip, unsigned char *res_mac_addr, unsigned char *req_ip);
};

class ArpCacheTable {
  private:
	struct mac_map_ip items[5];
	unsigned int max_capacity;

	int get_free_slot();
	bool is_free_slot(unsigned int slot);
	int locate_mac_addr(unsigned char *mac_addr);
	bool is_exists(unsigned char *mac_addr, unsigned char *ip_addr);
	int remove_less_use_item();

  public:
	ArpCacheTable();
	void set_item(unsigned int slot, unsigned char *mac_addr, unsigned char *ip_addr);
	const unsigned char* get_ip_addr(unsigned char *mac_addr);
	bool delete_item(unsigned char *mac_addr);
	void append(unsigned char *mac_addr, unsigned char *ip_addr);
	void print_all();
};

namespace {
	namespace arp {
		unsigned char mac_broadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
		unsigned char mac_zero[6] = {0, 0, 0, 0, 0, 0};
		unsigned short hw_type = 0x0001;
		unsigned short ipv4 = 0x0800;
		unsigned char hw_addr_size = 0x06;
		unsigned char proto_size = 0x04;
		unsigned short req_opcode = 0x0001;
		unsigned short rsp_opcode = 0x0002;
	};
}

extern unsigned char ip_addr[4];

void arp_request(unsigned char *target_ip);
void arp_response(unsigned char *src_mac_addr, unsigned char *res_mac_addr, unsigned char *req_ip);
void print_ip_addr(unsigned char *ip_addr);

#endif


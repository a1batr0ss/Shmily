#include <global.h>
#include <stdio.h>
#include <all_syscall.h>
#include "arp.h"
#include "ethernet.h"
#include "byte_order.h"

void ArpFactory::format_request_packet(unsigned char *data, unsigned char *mac_addr, unsigned char *ip_addr, unsigned char *target_ip)
{
	struct arp_packet *pkt = (struct arp_packet*)data;

	pkt->hw_type = swap_word(arp::hw_type);
	pkt->req_protocol = swap_word(arp::ipv4);
	pkt->hw_addr_size = arp::hw_addr_size;
	pkt->protocol_size = arp::proto_size;
	pkt->opcode = swap_word(arp::req_opcode);

	/* Or memcpy. */
	for (int i=0; i<6; i++) {
		pkt->src_mac_addr[i] = mac_addr[i];
	    pkt->dst_mac_addr[i] = 0;
	}

	for (int i=0; i<4; i++) {
		pkt->src_ip_addr[i] = ip_addr[i];
		pkt->target_ip[i] = target_ip[i];
	}
}

void ArpFactory::request(unsigned char *mac_addr, unsigned char *ip_addr, unsigned char *target_ip)
{
	unsigned char *data = (unsigned char*)malloc(sizeof(struct eth_packet) + sizeof(struct arp_packet));

	EthernetFactory::format_packet(data, mac_addr, arp::mac_broadcast, frame::next_is_arp);
	format_request_packet(data + sizeof(struct eth_packet), mac_addr, ip_addr, target_ip);

	struct packet p;
	p.size = sizeof(struct eth_packet) + sizeof(struct arp_packet);
	p.data = data;

	send_packet((unsigned int)&p);

	free(data);
}

void ArpFactory::format_reply_packet(unsigned char *data, unsigned char *mac_addr, unsigned char *ip_addr, unsigned char *res_mac_addr, unsigned char *req_ip)
{
	struct arp_packet *pkt = (struct arp_packet*)data;

	pkt->hw_type = swap_word(arp::hw_type);
	pkt->req_protocol = swap_word(arp::ipv4);
	pkt->hw_addr_size = arp::hw_addr_size;
	pkt->protocol_size = arp::proto_size;
	pkt->opcode = swap_word(arp::rsp_opcode);

	for (int i=0; i<6; i++) {
		pkt->src_mac_addr[i] = mac_addr[i];
	    pkt->dst_mac_addr[i] = res_mac_addr[i];  /* Target mac address. */
	}

	for (int i=0; i<4; i++) {
		pkt->src_ip_addr[i] = ip_addr[i];
		pkt->target_ip[i] = req_ip[i];
	}
}

/* No test! */
void ArpFactory::reply(unsigned char *mac_addr, unsigned char *ip_addr, unsigned char *src_mac_addr, unsigned char *res_mac_addr, unsigned char *req_ip)
{
	unsigned char *data = (unsigned char*)malloc(sizeof(struct eth_packet) + sizeof(struct arp_packet));

	EthernetFactory::format_packet(data, mac_addr, src_mac_addr, frame::next_is_arp);
	format_reply_packet(data + sizeof(struct ethernet), mac_addr, ip_addr, res_mac_addr, req_ip);

	struct packet p;
	p.size = sizeof(struct eth_packet) + sizeof(struct arp_packet);
	p.data = data;

	send_packet((unsigned int)&p);

	free(data);
}

bool is_same_mac_addr(unsigned char *mac_addr1, unsigned char *mac_addr2)
{
	for (int i=0; i<6; i++) {
		if (mac_addr1[i] != mac_addr2[i])
			return false;
	}
	return true;
}

ArpCacheTable::ArpCacheTable()
{
	/* Initialize the first item. */
	/* The arp cache table will be a global variable, the mac_addr is initialized in init_net. */
	this->max_capacity = 5;
}

void ArpCacheTable::set_item(unsigned int slot, unsigned char *mac_addr, unsigned char *ip_addr)
{
	for (int i=0; i<6; i++)
		this->items[slot].mac_addr[i] = mac_addr[i];

	for (int i=0; i<4; i++)
		this->items[slot].ip_addr[i] = ip_addr[i];
}

const unsigned char* ArpCacheTable::get_ip_addr(unsigned char *mac_addr)
{
	int i = locate_mac_addr(mac_addr);

	if (-1 != i) {
		this->items[i].ref++;  /* Even just for judge in append. For simply, I also increase it. */
		return this->items[i].ip_addr;
	}

	return NULL;
}

bool ArpCacheTable::delete_item(unsigned char *mac_addr)
{
	int i = locate_mac_addr(mac_addr);

	unsigned char nil_ip[4] = {0, 0, 0, 0};
	if (-1 != i)
		set_item(i, arp::mac_zero, nil_ip);
}

int ArpCacheTable::locate_mac_addr(unsigned char *mac_addr)
{
	for (int i=0; i<5; i++) {
		if (is_same_mac_addr(this->items[i].mac_addr, mac_addr))
			return i;
	}
	return -1;
}

int ArpCacheTable::get_free_slot()
{
	for (int i=0; i<5; i++) {
		if (is_free_slot(i))
			return i;
	}
	return -1;
}

bool ArpCacheTable::is_free_slot(unsigned int slot)
{
	struct mac_map_ip item = this->items[slot];

	/* Only check the mac address. */
	for (int i=0; i<6; i++) {
		if (0 != item.mac_addr[i])
			return false;
	}
	return true;
}

bool ArpCacheTable::is_exists(unsigned char *mac_addr, unsigned char *ip_addr)
{
	const unsigned char *ip_addr_in_tbl = get_ip_addr(mac_addr);
	if (NULL == ip_addr_in_tbl)
		return false;

	/* Is matching? */
	for (int i=0; i<4; i++) {
		if (ip_addr[i] != ip_addr_in_tbl[i])
			return false;
	}
	return true;
}

int ArpCacheTable::remove_less_use_item()
{
	int idx = 0;
	int less_use_cnt = this->items[0].ref;

	for (int i=1; i<5; i++) {
		if (this->items[i].ref < less_use_cnt) {
			idx = i;
			less_use_cnt = this->items[i].ref;
		}
	}

	return idx;
}

void ArpCacheTable::append(unsigned char *mac_addr, unsigned char *ip_addr)
{
	/* Already exists? */
	if (is_exists(mac_addr, ip_addr))
		return;

	int slot = get_free_slot();
	if (-1 == slot)
		slot = remove_less_use_item();

	if (-1 != slot)
		set_item(slot, mac_addr, ip_addr);
}

void print_mac_addr(unsigned char *mac_addr)
{
	for (int i=0; i<6; i++) {
		printf("%x", mac_addr[i]);
		if (5 != i)
			printf(":");
	}
}

void print_ip_addr(unsigned char *ip_addr)
{
	for (int i=0; i<4; i++) {
		printf("%d", ip_addr[i]);
		if (3 != i)
			printf(".");
	}
}

void ArpCacheTable::print_all()
{
	for (int i=0; i<5; i++) {
		if (!is_free_slot(i)) {
			print_mac_addr(this->items[i].mac_addr);
			printf(" -> ");
			print_ip_addr(this->items[i].ip_addr);
			printf("\n");
		}
	}
}


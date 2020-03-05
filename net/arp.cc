#include <global.h>
#include <stdio.h>
#include <all_syscall.h>
#include "arp.h"
#include "ethernet.h"
#include "byte_order.h"

extern char mac_addr[6];
unsigned char ip_addr[4] = {192, 168, 22, 44};

void init_arp_request(unsigned char *data, unsigned char *target_ip)
{
	struct arp_packet *pkt = (struct arp_packet*)data;

	pkt->hw_type = swap_word(arp::hw_type);
	pkt->req_protocol = swap_word(arp::ipv4);
	pkt->hw_addr_size = arp::hw_addr_size;
	pkt->protocol_size = arp::proto_size;
	pkt->opcode = swap_word(arp::req_opcode);

	for (int i=0; i<6; i++) {
		pkt->src_mac_addr[i] = mac_addr[i];
	    pkt->dst_mac_addr[i] = 0;
	}

	for (int i=0; i<4; i++) {
		pkt->src_ip_addr[i] = ip_addr[i];
		pkt->target_ip[i] = target_ip[i];
	}
}

void arp_request(unsigned char *target_ip)
{
	unsigned char *data = (unsigned char*)malloc(sizeof(struct eth_packet) + sizeof(struct arp_packet));

	init_ethernet_packet(data, arp::mac_broadcast, frame::next_is_arp);
	init_arp_request(data + sizeof(struct eth_packet), target_ip);

	struct packet p;
	p.size = sizeof(struct eth_packet) + sizeof(struct arp_packet);
	p.data = data;

	send_packet((unsigned int)&p);

	free(data);
}

void init_arp_response(unsigned char *data, unsigned char *res_mac_addr, unsigned char *req_ip)
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
void arp_response(unsigned char *src_mac_addr, unsigned char *res_mac_addr, unsigned char *req_ip)
{
	unsigned char *data = (unsigned char*)malloc(sizeof(struct eth_packet) + sizeof(struct arp_packet));

	init_ethernet_packet(data, src_mac_addr, frame::next_is_arp);
	init_arp_response(data + sizeof(struct ethernet), res_mac_addr, req_ip);

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

	if (-1 != i)
		return this->items[i].mac_addr;

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

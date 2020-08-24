#include <all_syscall.h>
#include <stdio.h>
#include "network_manager.h"

NetworkManager::NetworkManager()
{
	unsigned char mac_addr[6] = {0};
	unsigned char ip_addr[4] = {172, 17, 0, 2};

	get_mac_addr(mac_addr);

	this->net_ifaces[0].set_mac_addr(mac_addr);
	this->net_ifaces[0].set_ip_addr(ip_addr);
	this->net_ifaces[0].set_name("iface1");

	this->cur_iface = 0;

	this->net_ifaces[0].get_arp_table().set_item(0, mac_addr, ip_addr);
	this->ifaces_nr = 1;

	return;
}

void NetworkManager::create_iface(unsigned char *mac_addr, unsigned char *ip_addr) {}

NetworkInterface& NetworkManager::get_cur_ifaces()
{
	return this->net_ifaces[this->cur_iface];
}

void NetworkManager::show_all_ifaces()
{
	for (int i=0; i<this->ifaces_nr; i++)
		this->net_ifaces[i].show_info();
	return;
}


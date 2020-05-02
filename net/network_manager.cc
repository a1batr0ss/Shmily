#include <all_syscall.h>
#include <stdio.h>
#include "network_manager.h"

NetworkManager::NetworkManager()
{
	unsigned char mac_addr[6] = {0};
	unsigned char ip_addr[4] = {192, 168, 10, 9};

	get_mac_addr(mac_addr);

	this->net_ifaces[0].set_mac_addr(mac_addr);
	this->net_ifaces[0].set_ip_addr(ip_addr);

	this->cur_iface = 0;

	return;
}

void NetworkManager::create_iface(unsigned char *mac_addr, unsigned char *ip_addr) {}

NetworkInterface& NetworkManager::get_cur_ifaces()
{
	return this->net_ifaces[this->cur_iface];
}


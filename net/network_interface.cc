#include <string.h>
#include "network_interface.h"

NetworkInterface::NetworkInterface() {}

NetworkInterface::NetworkInterface(unsigned char *mac_addr, unsigned char *ip_addr)
{
	memcpy((char*)(this->mac_addr), (char*)mac_addr, 6);
	memcpy((char*)(this->ip_addr), (char*)ip_addr, 4);

	this->arp_tbl.set_item(0, mac_addr, ip_addr);
}

void NetworkInterface::set_mac_addr(unsigned char *mac_addr)
{
	memcpy((char*)(this->mac_addr), (char*)mac_addr, 6);
}

void NetworkInterface::set_ip_addr(unsigned char *ip_addr)
{
	memcpy((char*)(this->ip_addr), (char*)ip_addr, 4);
}
unsigned char* NetworkInterface::get_mac_addr()
{
	return this->mac_addr;
}

unsigned char* NetworkInterface::get_ip_addr()
{
	return this->ip_addr;
}

ArpCacheTable& NetworkInterface::get_arp_table()
{
	return this->arp_tbl;
}

RouteTable& NetworkInterface::get_route_table()
{
	return this->route_tbl;
}


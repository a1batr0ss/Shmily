#include <string.h>
#include <stdio.h>
#include "network_interface.h"

NetworkInterface::NetworkInterface()
{
	unsigned char default_gateway[4] = {172, 17, 0, 1};

	this->route_tbl.set_default_gateway(default_gateway);
}

NetworkInterface::NetworkInterface(unsigned char *mac_addr, unsigned char *ip_addr)
{
	memcpy((char*)(this->mac_addr), (char*)mac_addr, 6);
	memcpy((char*)(this->ip_addr), (char*)ip_addr, 4);

	this->arp_tbl.set_item(0, mac_addr, ip_addr);

	unsigned char default_gateway[4] = {172, 17, 0, 1};

	this->route_tbl.set_default_gateway(default_gateway);
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

unsigned char* NetworkInterface::get_default_gateway_ip()
{
	return this->route_tbl.get_default_gateway();
}

unsigned char* NetworkInterface::get_default_gateway_mac()
{
	unsigned char *gateway_ip = get_default_gateway_ip();

	/* The gateway map should exists in the table. Otherwise it would return null. */
	return this->arp_tbl.get_mac_addr(gateway_ip);
}


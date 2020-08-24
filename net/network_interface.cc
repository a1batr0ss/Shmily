#include <string.h>
#include <stdio.h>
#include "network_interface.h"

NetworkInterface::NetworkInterface()
{
	memset((char*)(this->iface_name), 0, 16);
	unsigned char default_gateway[4] = {172, 17, 0, 1};
	this->route_tbl.set_default_gateway(default_gateway);
}

NetworkInterface::NetworkInterface(unsigned char *mac_addr, unsigned char *ip_addr)
{
	memcpy((char*)(this->mac_addr), (char*)mac_addr, 6);
	memcpy((char*)(this->ip_addr), (char*)ip_addr, 4);
	memset((char*)(this->iface_name), 0, 16);

	this->arp_tbl.set_item(0, mac_addr, ip_addr);

	unsigned char default_gateway[4] = {172, 17, 0, 1};
	this->route_tbl.set_default_gateway(default_gateway);
}

void NetworkInterface::set_mac_addr(unsigned char *mac_addr)
{
	memcpy((char*)(this->mac_addr), (char*)mac_addr, 6);
}

void NetworkInterface::set_name(char *name)
{
	memcpy((char*)(this->iface_name), name, strlen(name));
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
	// printf("===>%x %x %x %x\n", gateway_ip[0], gateway_ip[1], gateway_ip[2], gateway_ip[3]);

	/* The gateway map should exists in the table. Otherwise it would return null. */
	return this->arp_tbl.get_mac_addr(gateway_ip);
}

void NetworkInterface::config_iface(unsigned int ip_uint, unsigned int gateway_uint)
{
	unsigned char new_ip[4] = {0};
	unsigned char new_gateway[4] = {0};

	new_ip[0] = (ip_uint & 0xff000000) >> 24;
	new_ip[1] = (ip_uint & 0x00ff0000) >> 16;
	new_ip[2] = (ip_uint & 0x0000ff00) >> 8;
	new_ip[3] = ip_uint & 0x000000ff;

	set_ip_addr(new_ip);
	/* set_gateway(), UNUSED now. */
	return;

}

void NetworkInterface::show_info()
{
	printf("%s:  ", this->iface_name);

	printf("inet: ");
	print_ip_addr(this->ip_addr);
	printf("\n");

	printf("ether: ");
	print_mac_addr(this->mac_addr);
	printf("\n");

	return;
}


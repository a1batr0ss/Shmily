#ifndef __NET_NETWORK_INTERFACE_H__
#define __NET_NETWORK_INTERFACE_H__

#include "arp.h"
#include "route_table.h"

class NetworkInterface {
  private:
	unsigned char mac_addr[6];
	unsigned char ip_addr[4];
	ArpCacheTable arp_tbl;
	RouteTable route_tbl;

  public:
	NetworkInterface();
	NetworkInterface(unsigned char *mac_addr, unsigned char *ip_addr);
	void set_mac_addr(unsigned char *mac_addr);
	void set_ip_addr(unsigned char *ip_addr);
	unsigned char* get_mac_addr();
	unsigned char* get_ip_addr();

	ArpCacheTable& get_arp_table();
	RouteTable& get_route_table();

	unsigned char* get_default_gateway_ip();
	unsigned char* get_default_gateway_mac();
};

#endif


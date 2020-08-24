#ifndef __NET_NETWORK_MANAGER_H__
#define __NET_NETWORK_MANAGER_H__

#include "network_interface.h"

class NetworkManager {
  private:
	NetworkInterface net_ifaces[5];
	unsigned char cur_iface;
	unsigned char ifaces_nr;

  public:
	NetworkManager();
	void create_iface(unsigned char *mac_addr, unsigned char *ip_addr);
	NetworkInterface& get_cur_ifaces();
	void show_all_ifaces();
};

#endif


#include <stdio.h>
#include "route_table.h"
#include "arp.h"

RouteTable::RouteTable()
{
	/* Just initialize the default gateway.(The first item.) */
	for (int i=0; i<4; i++) {
		this->items[0].destination[i] = 0;
		this->items[0].gateway[i] = 0;
		this->items[0].mask[i] = 0;
	}
}

const unsigned char* RouteTable::get_default_gateway()
{
	return this->items[0].gateway;
}

void RouteTable::set_default_gateway(unsigned char *default_gateway_)
{
	for (int i=0; i<4; i++)
		this->items[0].gateway[i] = default_gateway_[i];
}

void RouteTable::print_all()
{
	/* If the mask[0] is 0, I think it's an empty item. */
	for (int i=0; i<4; i++) {
		if ((0 != this->items[i].mask[0]) || (0 == i)) {
			print_ip_addr(this->items[i].destination);
			printf(" - ");
			print_ip_addr(this->items[i].gateway);
			printf(" - ");
			print_ip_addr(this->items[i].mask);
			printf("\n");
		}
	}
}


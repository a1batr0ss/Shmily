#ifndef __NET_ROUTE_TABLE_H__
#define __NET_ROUTE_TABLE_H__

struct route_tbl_item {
	unsigned char destination[4];
	unsigned char gateway[4];
	unsigned char mask[4];
};

/* Now just use the default gateway. */
class RouteTable {
  private:
	struct route_tbl_item items[4];  /* The first is the default gateway. */

  public:
	RouteTable();
	const unsigned char* get_default_gateway();
	void set_default_gateway(unsigned char *default_gateway_);

	void print_all();
};

#endif


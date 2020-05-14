#ifndef __NET_IP_H__
#define __NET_IP_H__

struct ip_packet {
	unsigned char header_len    :4;
	unsigned char version       :4;  /* byte order! */
	unsigned char diff_services;
	unsigned short total_len;
	unsigned short identification;
	unsigned short flags;
	unsigned char ttl;
	unsigned char next_protocol;
	unsigned short header_checksum;
	unsigned char src_ip[4];
	unsigned char dst_ip[4];
} __attribute__((packed));

class IPv4Factory {
  private:
	static unsigned short identifier;

  public:
	static void format_packet(unsigned char *data, unsigned char *ip_addr, unsigned char *target_ip, unsigned short ident, unsigned char next_proto, unsigned short total_len);
};

namespace {
	namespace ip {
		unsigned short no_fragment = 0x4000;
		unsigned char default_ttl = 0x99;
	};
};

unsigned short generate_checksum(unsigned short *pkt, unsigned int len);

#endif


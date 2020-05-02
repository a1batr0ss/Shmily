#ifndef __NET_ICMP_H__
#define __NET_ICMP_H__

struct icmp_packet {
	unsigned char type;
	unsigned char code;
	unsigned short checksum;
	unsigned short identifier;
	unsigned short sequence_num;
	unsigned char timestamp[8];
	unsigned char data[48];
} __attribute__((packed));

namespace {
	namespace icmp {
		const unsigned char ICMP_REQUEST = 0x08;
		const unsigned char ICMP_REPLY = 0x0;
	};
};

class ICMPFactory {
  private:
	static bool need_output;  /* Simply decide to print. A reqeust before, do printing. */

	static void print_reply(struct icmp_packet *pkt);

  public:
	static void format_packet(unsigned char *data, unsigned char type);
	static void resolve_packet(unsigned char *data);
	static void request(unsigned char *mac_addr, unsigned char *ip_addr, unsigned char *target_ip, unsigned char *target_mac_addr);
	static void reply(unsigned char *mac_addr, unsigned char *ip_addr, unsigned char *target_ip, unsigned char *target_mac_addr);

};


#endif


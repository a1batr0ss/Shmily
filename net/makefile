GG = g++
LD = ld
BUILD_DIR = ../build

GG_FLAGS = -c -m32 -fno-use-cxa-atexit -fno-rtti -fno-stack-protector -I ../lib -I ../include
LD_FLAGS = -m elf_i386

all: $(BUILD_DIR)/net

$(BUILD_DIR)/net: $(BUILD_DIR)/net_main.cc.o $(BUILD_DIR)/stdio.cc.o $(BUILD_DIR)/string.cc.o $(BUILD_DIR)/print.o $(BUILD_DIR)/syscall.cc.o $(BUILD_DIR)/all_syscall.cc.o $(BUILD_DIR)/byte_order.cc.o $(BUILD_DIR)/arp.cc.o $(BUILD_DIR)/ethernet.cc.o $(BUILD_DIR)/ip.cc.o $(BUILD_DIR)/icmp.cc.o $(BUILD_DIR)/udp.cc.o $(BUILD_DIR)/dns.cc.o $(BUILD_DIR)/time.cc.o $(BUILD_DIR)/indirect_ring_buffer.cc.o $(BUILD_DIR)/route_table.cc.o $(BUILD_DIR)/start.o $(BUILD_DIR)/start.cc.o $(BUILD_DIR)/protocol_stack.cc.o $(BUILD_DIR)/network_interface.cc.o $(BUILD_DIR)/network_manager.cc.o $(BUILD_DIR)/tcp.cc.o
	$(LD) -T link.ld $(LD_FLAGS) $^ -o $@

$(BUILD_DIR)/net_main.cc.o: main.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/byte_order.cc.o: byte_order.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/arp.cc.o: arp.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/ethernet.cc.o: ethernet.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/ip.cc.o: ip.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/icmp.cc.o: icmp.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/udp.cc.o: udp.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/dns.cc.o: dns.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/route_table.cc.o: route_table.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/protocol_stack.cc.o: protocol_stack.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/network_interface.cc.o: network_interface.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/network_manager.cc.o: network_manager.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/tcp.cc.o: tcp.cc
	$(GG) $(GG_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/net $(BUILD_DIR)/net.cc.o $(BUILD_DIR)/net_main.cc.o $(BUILD_DIR)/arp.cc.o $(BUILD_DIR)/byte_order.cc.o $(BUILD_DIR)/ethernet.cc.o $(BUILD_DIR)/ip.cc.o $(BUILD_DIR)/icmp.cc.o $(BUILD_DIR)/udp.cc.o $(BUILD_DIR)/dns.cc.o $(BUILD_DIR)/protocol_stack.cc.o $(BUILD_DIR)/network_interface.cc.o $(BUILD_DIR)/network_manager.cc.o $(BUILD_DIR)/tcp.cc.o


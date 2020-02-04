GG = g++
LD = ld
BUILD_DIR = ../build

GG_FLAGS = -c -m32 -fno-stack-protector -I ../lib 
LD_FLAGS = -e main -m elf_i386 -Ttext 0x50000

all: $(BUILD_DIR)/net

$(BUILD_DIR)/net: $(BUILD_DIR)/net_main.cc.o $(BUILD_DIR)/stdio.cc.o $(BUILD_DIR)/string.cc.o $(BUILD_DIR)/print.o $(BUILD_DIR)/syscall.cc.o $(BUILD_DIR)/all_syscall.cc.o $(BUILD_DIR)/byte_order.cc.o $(BUILD_DIR)/arp.cc.o $(BUILD_DIR)/net.cc.o
	$(LD) $(LD_FLAGS) $^ -o $@

$(BUILD_DIR)/net_main.cc.o: main.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/byte_order.cc.o: byte_order.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/arp.cc.o: arp.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/net.cc.o: net.cc
	$(GG) $(GG_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/net $(BUILD_DIR)/net.cc.o


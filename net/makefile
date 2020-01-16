GG = g++
LD = ld
BUILD_DIR = ../build

GG_FLAGS = -c -m32 -fno-stack-protector -I ../lib 
LD_FLAGS = -e main -m elf_i386 -Ttext 0x50000

all: $(BUILD_DIR)/net

$(BUILD_DIR)/net: $(BUILD_DIR)/net.cc.o $(BUILD_DIR)/stdio.cc.o $(BUILD_DIR)/string.cc.o $(BUILD_DIR)/print.o $(BUILD_DIR)/ne2k.cc.o
	$(LD) $(LD_FLAGS) $^ -o $@

$(BUILD_DIR)/ne2k.cc.o: ne2k.cc
	$(GG) $(GG_FLAGS) $< -o $@	

$(BUILD_DIR)/net.cc.o: main.cc
	$(GG) $(GG_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/net $(BUILD_DIR)/net.cc.o $(BUILD_DIR)/ne2k.cc.o

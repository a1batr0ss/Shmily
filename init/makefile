AS = nasm
LD = ld
GG = g++
BUILD_DIR = ../build

AS_FLAGS = -f elf
GG_FLAGS = -c -m32 -I ../lib
LD_FLAGS = -Ttext 0x5000 -e main -m elf_i386

all: $(BUILD_DIR)/init.bin $(BUILD_DIR)/init.out

$(BUILD_DIR)/init.bin: init.s
	$(AS) $< -o $@

$(BUILD_DIR)/init.out: $(BUILD_DIR)/init.cc.o $(BUILD_DIR)/page.cc.o $(BUILD_DIR)/string.cc.o $(BUILD_DIR)/print.o
	$(LD) $(LD_FLAGS) $^ -o $@

$(BUILD_DIR)/init.cc.o: main.cc
	$(GG) $(GG_FLAGS) $^ -o $@

$(BUILD_DIR)/page.cc.o: page.cc
	$(GG) $(GG_FLAGS) $^ -o $@

clean:
	rm -rf $(BUILD_DIR)/init.bin $(BUILD_DIR)/init.out $(BUILD_DIR)/init.cc.o $(BUILD_DIR)/page.cc.o


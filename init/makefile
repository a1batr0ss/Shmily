ASM = nasm
LD = ld
GG = g++
BUILD_DIR = ../build

GG_FLAGS = -c -m32
LD_FLAGS = -Ttext 0x5000 -e main -m elf_i386

all: $(BUILD_DIR)/init.bin $(BUILD_DIR)/init.out

$(BUILD_DIR)/init.bin: init.s
	$(ASM) $< -o $@

$(BUILD_DIR)/init.out: $(BUILD_DIR)/init.cc.o $(BUILD_DIR)/page.cc.o $(BUILD_DIR)/string.cc.o
	$(LD) $(LD_FLAGS) $^ -o $@

$(BUILD_DIR)/init.cc.o: main.cc
	$(GG) $(GG_FLAGS) $^ -o $@

$(BUILD_DIR)/page.cc.o: page.cc
	$(GG) $(GG_FLAGS) $^ -o $@

$(BUILD_DIR)/string.cc.o: string.cc
	$(GG) $(GG_FLAGS) $^ -o $@

clean:
	rm -rf $(BUILD_DIR)/init.bin $(BUILD_DIR)/init.out $(BUILD_DIR)/init.cc.o $(BUILD_DIR)/page.cc.o $(BUILD_DIR)/string.cc.o


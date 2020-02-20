AS = nasm
LD = ld
GG = g++
BUILD_DIR = ../build

AS_FLAGS = -f elf
GG_FLAGS = -c -m32 -fno-stack-protector -I ../lib -I ../include 
LD_FLAGS = -Ttext 0x10000 -e main -m elf_i386

all: $(BUILD_DIR)/init.bin $(BUILD_DIR)/kernel

$(BUILD_DIR)/init.bin: init.s
	$(AS) $< -o $@

$(BUILD_DIR)/kernel: $(BUILD_DIR)/init.cc.o $(BUILD_DIR)/page.cc.o $(BUILD_DIR)/string.cc.o $(BUILD_DIR)/print.o $(BUILD_DIR)/interrupt.cc.o $(BUILD_DIR)/interrupt.o $(BUILD_DIR)/timer.cc.o $(BUILD_DIR)/process.cc.o $(BUILD_DIR)/all_syscall.cc.o $(BUILD_DIR)/bitmap.cc.o $(BUILD_DIR)/sync.cc.o $(BUILD_DIR)/ipc.cc.o $(BUILD_DIR)/stdio.cc.o $(BUILD_DIR)/syscall.o $(BUILD_DIR)/syscall_init.cc.o $(BUILD_DIR)/cmos.cc.o
	$(LD) $(LD_FLAGS) $^ -o $@

$(BUILD_DIR)/init.cc.o: main.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/page.cc.o: page.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/interrupt.cc.o: interrupt.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/timer.cc.o: timer.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/ipc.cc.o: ipc.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/sync.cc.o: sync.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/process.cc.o: process.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/syscall_init.cc.o: syscall_init.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/cmos.cc.o: cmos.cc
	$(GG) $(GG_FLAGS) $< -o $@
 
$(BUILD_DIR)/interrupt.o: interrupt.s
	$(AS) $(AS_FLAGS) $< -o $@

$(BUILD_DIR)/syscall.o: syscall.s
	$(AS) $(AS_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/init.bin $(BUILD_DIR)/init.out $(BUILD_DIR)/init.cc.o $(BUILD_DIR)/page.cc.o $(BUILD_DIR)/ipc.cc.o $(BUILD_DIR)syscall_init.cc.o $(BUILD_DIR)/syscall.o $(BUILD_DIR)/process.cc.o $(BUILD_DIR)/cmos.cc.o $(BUILD_DIR)/interrupt.cc.o $(BUILD_DIR)/kernel $(BUILD_DIR)/sync.cc.o


GG = g++
LD = ld
BUILD_DIR = ../build

GG_FLAGS = -c -m32 -fno-use-cxa-atexit -fno-rtti -fno-stack-protector -I ../lib -I ../include
LD_FLAGS = -m elf_i386

all: $(BUILD_DIR)/dr

$(BUILD_DIR)/dr: $(BUILD_DIR)/dr.cc.o $(BUILD_DIR)/stdio.cc.o $(BUILD_DIR)/print.o $(BUILD_DIR)/string.cc.o $(BUILD_DIR)/syscall.cc.o $(BUILD_DIR)/keyboard.cc.o $(BUILD_DIR)/disk.cc.o $(BUILD_DIR)/all_syscall.cc.o $(BUILD_DIR)/ne2k.cc.o $(BUILD_DIR)/ring_buffer.cc.o $(BUILD_DIR)/time.cc.o $(BUILD_DIR)/indirect_ring_buffer.cc.o  $(BUILD_DIR)/start.o $(BUILD_DIR)/start.cc.o
	$(LD) -T link.ld $(LD_FLAGS) $^ -o $@

$(BUILD_DIR)/dr.cc.o: main.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/keyboard.cc.o: keyboard.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/disk.cc.o: disk.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/ne2k.cc.o: ne2k.cc
	$(GG) $(GG_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/dr $(BUILD_DIR)/disk.cc.o $(BUILD_DIR)/keyboard.cc.o $(BUILD_DIR)/dr.cc.o $(BUILD_DIR)/ne2k.cc.o


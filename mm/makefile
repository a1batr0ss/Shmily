GG = g++
LD = ld
BUILD_DIR = ../build

GG_FLAGS = -c -m32 -fno-use-cxa-atexit -fno-rtti -fno-stack-protector -I ../lib -I ../include
LD_FLAGS = -m elf_i386

all: $(BUILD_DIR)/mm

$(BUILD_DIR)/mm: $(BUILD_DIR)/mm.cc.o $(BUILD_DIR)/memory.cc.o $(BUILD_DIR)/syscall.cc.o  $(BUILD_DIR)/bitmap.cc.o $(BUILD_DIR)/string.cc.o $(BUILD_DIR)/stdio.cc.o $(BUILD_DIR)/print.o $(BUILD_DIR)/swar.cc.o $(BUILD_DIR)/doublebit_map.cc.o $(BUILD_DIR)/cycle_list.cc.o $(BUILD_DIR)/small_mem_header.cc.o  $(BUILD_DIR)/start.o $(BUILD_DIR)/start.cc.o
	$(LD) -T link.ld $(LD_FLAGS) $^ -o $@

$(BUILD_DIR)/memory.cc.o: memory.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/mm.cc.o: main.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/swar.cc.o: swar.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/doublebit_map.cc.o: doublebit_map.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/small_mem_header.cc.o: small_mem_header.cc
	$(GG) $(GG_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/memory.cc.o $(BUILD_DIR)/mm.cc.o $(BUILD_DIR)/mm $(BUILD_DIR)/swar.cc.o $(BUILD_DIR)/doublebit_map.cc.o $(BUILD_DIR)/small_mem_header.cc.o


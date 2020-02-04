GG = g++
LD = ld
BUILD_DIR = ../build

GG_FLAGS = -c -m32 -fno-stack-protector -I ../lib 
LD_FLAGS = -e main -m elf_i386 -Ttext 0x40000

all: $(BUILD_DIR)/dr

$(BUILD_DIR)/dr: $(BUILD_DIR)/dr.cc.o $(BUILD_DIR)/stdio.cc.o $(BUILD_DIR)/print.o $(BUILD_DIR)/string.cc.o $(BUILD_DIR)/syscall.cc.o $(BUILD_DIR)/keyboard.cc.o $(BUILD_DIR)/disk.cc.o $(BUILD_DIR)/all_syscall.cc.o $(BUILD_DIR)/ne2k.cc.o
	$(LD) $(LD_FLAGS) $^ -o $@

$(BUILD_DIR)/dr.cc.o: main.cc
	$(GG) $(GG_FLAGS) $< -o $@	

$(BUILD_DIR)/keyboard.cc.o: keyboard.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/disk.cc.o: disk.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/ne2k.cc.o: ne2k.cc
	$(GG) $(GG_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/dr $(BUILD_DIR)/disk.cc.o $(BUILD_DIR)/keyboard.cc.o $(BUILD_DIR)/dr.cc.o

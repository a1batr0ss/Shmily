GG = g++
LD = ld
BUILD_DIR = ../build

GG_FLAGS = -c -m32 -fno-stack-protector -I ../lib -I ../include
LD_FLAGS = -e main -m elf_i386 -Ttext 0x30000

all: $(BUILD_DIR)/fs

$(BUILD_DIR)/fs: $(BUILD_DIR)/fs_main.cc.o $(BUILD_DIR)/fs.cc.o $(BUILD_DIR)/stdio.cc.o $(BUILD_DIR)/print.o $(BUILD_DIR)/string.cc.o $(BUILD_DIR)/syscall.cc.o $(BUILD_DIR)/all_syscall.cc.o $(BUILD_DIR)/inode.cc.o $(BUILD_DIR)/bitmap.cc.o $(BUILD_DIR)/dir.cc.o $(BUILD_DIR)/file.cc.o $(BUILD_DIR)/time.cc.o $(BUILD_DIR)/protect.cc.o
	$(LD) $(LD_FLAGS) $^ -o $@

$(BUILD_DIR)/fs_main.cc.o: main.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/fs.cc.o: fs.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/inode.cc.o: inode.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/dir.cc.o: dir.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/file.cc.o: file.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/protect.cc.o: protect.cc
	$(GG) $(GG_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/fs $(BUILD_DIR)/fs.cc.o $(BUILD_DIR)/fs_main.cc.o $(BUILD_DIR)/inode.cc.o $(BUILD_DIR)/file.cc.o $(BUILD_DIR)/protect.cc.o


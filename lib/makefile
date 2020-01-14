AS = nasm
GG = g++
BUILD_DIR = ../build

GG_FLAGS = -c -m32 -I . -fno-stack-protector
AS_FLAGS = -f elf

all: $(BUILD_DIR)/print.o $(BUILD_DIR)/string.cc.o $(BUILD_DIR)/bitmap.cc.o $(BUILD_DIR)/stdio.cc.o $(BUILD_DIR)/syscall.cc.o $(BUILD_DIR)/all_syscall.cc.o

$(BUILD_DIR)/print.o: print.s
	$(AS) $(AS_FLAGS) $< -o $@

$(BUILD_DIR)/string.cc.o: string.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/bitmap.cc.o: bitmap.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/stdio.cc.o: stdio.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/syscall.cc.o: syscall.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/all_syscall.cc.o: all_syscall.cc
	$(GG) $(GG_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/print.o $(BUILD_DIR)/string.cc.o $(BUILD_DIR)/bitmap.cc.o $(BUILD_DIR)/stdio.cc.o $(BUILD_DIR)/syscall.cc.o $(BUILD_DIR)/all_process.cc.o


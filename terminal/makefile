GG = g++
LD = ld
BUILD_DIR = ../build

GG_FLAGS = -c -m32 -fno-stack-protector -I ../lib -I ../include
LD_FLAGS = -e main -m elf_i386 -Ttext 0x30000

all: $(BUILD_DIR)/terminal

$(BUILD_DIR)/terminal: $(BUILD_DIR)/terminal_main.cc.o $(BUILD_DIR)/stdio.cc.o $(BUILD_DIR)/string.cc.o $(BUILD_DIR)/syscall.cc.o $(BUILD_DIR)/all_syscall.cc.o $(BUILD_DIR)/print.o $(BUILD_DIR)/ring_buffer.cc.o $(BUILD_DIR)/terminal.cc.o $(BUILD_DIR)/builtin_cmd.cc.o
	$(LD) $(LD_FLAGS) $^ -o $@

$(BUILD_DIR)/terminal_main.cc.o: main.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/terminal.cc.o: terminal.cc
	$(GG) $(GG_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/terminal_main.cc.o $(BUILD_DIR)/terminal $(BUILD_DIR)/terminal.cc.o


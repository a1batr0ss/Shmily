GG = g++
LD = ld
BUILD_DIR = ../build

GG_FLAGS = -c -m32 -fno-use-cxa-atexit -fno-rtti -fno-stack-protector -I ../lib -I ../include
LD_FLAGS = -m elf_i386

all: $(BUILD_DIR)/terminal

$(BUILD_DIR)/terminal: $(BUILD_DIR)/terminal_main.cc.o $(BUILD_DIR)/stdio.cc.o $(BUILD_DIR)/string.cc.o $(BUILD_DIR)/syscall.cc.o $(BUILD_DIR)/all_syscall.cc.o $(BUILD_DIR)/print.o $(BUILD_DIR)/ring_buffer.cc.o $(BUILD_DIR)/terminal.cc.o $(BUILD_DIR)/builtin_cmd.cc.o $(BUILD_DIR)/time.cc.o $(BUILD_DIR)/user.cc.o $(BUILD_DIR)/hahash.cc.o  $(BUILD_DIR)/start.o $(BUILD_DIR)/start.cc.o
	$(LD) -T link.ld $(LD_FLAGS) $^ -o $@

$(BUILD_DIR)/terminal_main.cc.o: main.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/terminal.cc.o: terminal.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/builtin_cmd.cc.o: builtin_cmd.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/user.cc.o: user.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/hahash.cc.o: hahash.cc
	$(GG) $(GG_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/terminal_main.cc.o $(BUILD_DIR)/terminal $(BUILD_DIR)/terminal.cc.o $(BUILD_DIR)/user.cc.o $(BUILD_DIR)/builtin_cmd.cc.o $(BUILD_DIR)/hahash.cc.o


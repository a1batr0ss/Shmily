AS = nasm
GG = g++
BUILD_DIR = ../build

GG_FLAGS = -c -m32 -I . -I ../include -fno-stack-protector
AS_FLAGS = -f elf

all: $(BUILD_DIR)/print.o $(BUILD_DIR)/string.cc.o $(BUILD_DIR)/bitmap.cc.o $(BUILD_DIR)/stdio.cc.o $(BUILD_DIR)/syscall.cc.o $(BUILD_DIR)/all_syscall.cc.o $(BUILD_DIR)/ring_buffer.cc.o $(BUILD_DIR)/time.cc.o $(BUILD_DIR)/cycle_list.cc.o $(BUILD_DIR)/indirect_ring_buffer.cc.o $(BUILD_DIR)/start.o $(BUILD_DIR)/start.cc.o $(BUILD_DIR)/start.o $(BUILD_DIR)/start.cc.o $(BUILD_DIR)/net_utilies.cc.o

$(BUILD_DIR)/start.o: start.s
	$(AS) $(AS_FLAGS) $< -o $@

$(BUILD_DIR)/print.o: print.s
	$(AS) $(AS_FLAGS) $< -o $@

$(BUILD_DIR)/start.cc.o: start.cc
	$(GG) $(GG_FLAGS) $< -o $@

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

$(BUILD_DIR)/ring_buffer.cc.o: ring_buffer.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/time.cc.o: time.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/cycle_list.cc.o: cycle_list.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/indirect_ring_buffer.cc.o: indirect_ring_buffer.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/net_utilies.cc.o: net_utilies.cc
	$(GG) $(GG_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/print.o $(BUILD_DIR)/string.cc.o $(BUILD_DIR)/bitmap.cc.o $(BUILD_DIR)/stdio.cc.o $(BUILD_DIR)/syscall.cc.o $(BUILD_DIR)/all_process.cc.o $(BUILD_DIR)/time.cc.o $(BUILD_DIR)/cycle_list.cc.o $(BUILD_DIR)/indirect_ring_buffer.cc.o $(BUILD_DIR)/net_utilies.cc.o


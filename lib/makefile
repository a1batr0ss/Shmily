AS = nasm
GG = g++
BUILD_DIR = ../build

GG_FLAGS = -c -m32 -I ./
AS_FLAGS = -f elf

all: $(BUILD_DIR)/print.o $(BUILD_DIR)/string.cc.o $(BUILD_DIR)/bitmap.cc.o

$(BUILD_DIR)/print.o: print.s
	$(AS) $(AS_FLAGS) $< -o $@

$(BUILD_DIR)/string.cc.o: string.cc
	$(GG) $(GG_FLAGS) $< -o $@

$(BUILD_DIR)/bitmap.cc.o: bitmap.cc
	$(GG) $(GG_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/print.o $(BUILD_DIR)/string.cc.o


ASM = nasm
BUILD_DIR = ../build

all: $(BUILD_DIR)/boot.bin

$(BUILD_DIR)/boot.bin: boot.s
	$(ASM) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/boot.bin


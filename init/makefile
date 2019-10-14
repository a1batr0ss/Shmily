ASM = nasm
BUILD_DIR = ../build

all: $(BUILD_DIR)/init.bin

$(BUILD_DIR)/init.bin: init.s
	$(ASM) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/boot.bin


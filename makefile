BUILD_DIR = ./build
DEBUGGER=0

all: install_boot install_init
	if [ $(DEBUGGER) == 0 ]; then echo "c" | bochs -f ./bochs/config.bcs; else bochs -f ./bochs/config.bcs; fi

install_boot: $(BUILD_DIR)/boot.bin
	dd if=./build/boot.bin of=./bochs/testdisk.img bs=512 count=1 conv=notrunc

install_init: $(BUILD_DIR)/init.bin
	dd if=./build/init.bin of=./bochs/testdisk.img bs=512 count=1 seek=1 conv=notrunc


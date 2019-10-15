BUILD_DIR = ./build
DEBUGGER=0

all: install_boot install_init
	if [ $(DEBUGGER) == 0 ]; then echo "c" | bochs -f ./bochs/config.bcs; else bochs -f ./bochs/config.bcs; fi

install_boot: $(BUILD_DIR)/boot.bin
	dd if=./build/boot.bin of=./bochs/testdisk.img bs=512 count=1 conv=notrunc

install_init: $(BUILD_DIR)/init.bin $(BUILD_DIR)/init.out
	dd if=./build/init.bin of=./bochs/testdisk.img bs=512 count=1 seek=1 conv=notrunc
	dd if=./build/init.out of=./bochs/testdisk.img bs=512 count=20 seek=5 conv=notrunc


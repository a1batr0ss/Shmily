BUILD_DIR = ./build
DEBUGGER=1

all: install_boot install_kernel
	if [ $(DEBUGGER) == 0 ]; then echo "c" | sudo bochs -f ./bochs/config.bcs; else sudo bochs -f ./bochs/config.bcs; fi

install_boot: $(BUILD_DIR)/boot.bin
	dd if=./build/boot.bin of=./bochs/testdisk.img bs=512 count=1 conv=notrunc

install_kernel: $(BUILD_DIR)/init.bin $(BUILD_DIR)/image
	dd if=./build/init.bin of=./bochs/testdisk.img bs=512 count=1 seek=1 conv=notrunc
	dd if=./build/image of=./bochs/testdisk.img bs=512 count=150 seek=5 conv=notrunc


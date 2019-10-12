BUILD_DIR = ./build
DEBUGGER=0

all: install_boot
	if [ $(DEBUGGER) == 0 ]; then echo "c" | bochs -f ./bochs/config.bcs; else bochs -f ./bochs/config.bcs; fi

install_boot: $(BUILD_DIR)/boot.bin
	dd if=./build/boot.bin of=./bochs/testdisk.img bs=512 count=1 conv=notrunc


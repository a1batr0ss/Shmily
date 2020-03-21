BUILD_DIR = ./build
DISK_DIR = ./bochs
DEBUGGER=0

all: check_disk check_build_dir build_all install_boot install_kernel
	if [ $(DEBUGGER) == 0 ]; then echo "c" | sudo bochs -f ./bochs/config.bcs; else sudo bochs -f ./bochs/config.bcs; fi

check_disk:
	./make_disk.sh

check_build_dir:
	if [ ! -d $(BUILD_DIR) ]; then mkdir $(BUILD_DIR); fi

build_all:
	./build_all.sh

install_boot: $(BUILD_DIR)/boot.bin
	dd if=./build/boot.bin of=./bochs/testdisk.img bs=512 count=1 conv=notrunc

install_kernel: $(BUILD_DIR)/init.bin $(BUILD_DIR)/image
	dd if=./build/init.bin of=./bochs/testdisk.img bs=512 count=1 seek=1 conv=notrunc
	dd if=./build/image of=./bochs/testdisk.img bs=512 count=420 seek=5 conv=notrunc

clean:
	rm -rf $(BUILD_DIR)/* $(DISK_DIR)/testdisk.img $(DISK_DIR)/fs_test.img


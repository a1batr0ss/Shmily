BUILD_DIR = ./build
DISK_DIR = ./bochs
DEBUGGER=0
VBOX_OS_NAME = Shmily

all: check_disk check_build_dir build_all install_boot install_kernel
	if [ $(DEBUGGER) == 0 ]; then echo "c" | sudo bochs -f ./bochs/config.bcs; else sudo bochs -f ./bochs/config.bcs; fi

qemu:
	qemu-system-i386 -m 32 -hda bochs/testdisk.img -hdb bochs/fs_test.img -boot d

vbox:
	qemu-img convert -f raw -O vdi bochs/testdisk.img bochs/testdisk.vdi
	qemu-img convert -f raw -O vdi bochs/fs_test.img bochs/fs_test.vdi
	VBoxManage createvm --name $(VBOX_OS_NAME) --register
	VBoxManage modifyvm $(VBOX_OS_NAME) --ostype other
	VBoxManage modifyvm $(VBOX_OS_NAME) --memory 32
	VBoxManage storagectl $(VBOX_OS_NAME) --name IDE --add ide --controller PIIX4 --bootable on
	VBoxManage storageattach $(VBOX_OS_NAME) --storagectl IDE --port 0 --device 0 --type hdd --medium bochs/testdisk.vdi
	VBoxManage storageattach $(VBOX_OS_NAME) --storagectl IDE --port 1 --device 0 --type hdd --medium bochs/fs_test.vdi
	VBoxManage modifyvm $(VBOX_OS_NAME) --nic1 natnetwork
	VBoxManage startvm $(VBOX_OS_NAME)

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
	dd if=./build/image of=./bochs/testdisk.img bs=512 count=450 seek=5 conv=notrunc

clean_vbox:
	rm -rf bochs/testdisk.vdi bochs/fs_test.vdi
	VBoxManage unregistervm $(VBOX_OS_NAME) --delete

clean:
	rm -rf $(BUILD_DIR)/* $(DISK_DIR)/testdisk.img $(DISK_DIR)/fs_test.img


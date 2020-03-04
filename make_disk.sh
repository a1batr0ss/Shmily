if [ ! -f "./bochs/testdisk.img" ]; then
	bximage -hd -mode="flat" -size=60M -q testdisk.img
	mv ./testdisk.img ./bochs/testdisk.img
fi

if [ ! -f "./bochs/fs_test.img" ]; then
bximage -hd -mode="flat" -size=40M -q fs_test.img

fdisk fs_test.img << END
n
p
1
2048
20480
n
e
2
22528
81647
n
24576
40000
n
43008
81647
w
END

mv ./fs_test.img ./bochs/fs_test.img
fi


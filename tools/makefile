GG = g++

all:  build_image

exec:
	./build_image

build_image: build_img.cc elf.cc
	$(GG) $^ -o $@

clean:
	rm -rf build_image


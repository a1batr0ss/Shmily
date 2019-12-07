GG = g++

all:  build_image
	./build_image

exec:
	./build_image

build_image: build_img.cc
	$(GG) $^ -o $@

clean:
	rm -rf build_image


BUILD_DIR = ../build
GG = g++

all:  $(BUILD_DIR)/build_image
	$(BUILD_DIR)/build_image

exec:
	$(BUILD_DIR)/build_image

$(BUILD_DIR)/build_image: build_img.cc
	$(GG) $^ -o $@

clean:
	rm -rf $(BUILD_DIR)/build_image


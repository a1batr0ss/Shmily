GG = g++
BUILD_DIR = ../build

GG_FLAGS = -c -m32 -fno-stack-protector -I ../lib 

all: $(BUILD_DIR)/filesystem_manager.cc.o

$(BUILD_DIR)/filesystem_manager.cc.o: manager.cc
	$(GG) $(GG_FLAGS) $< -o $@	

clean:
	rm -rf $(BUILD_DIR)/filesystem_manager.cc.o


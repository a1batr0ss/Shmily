GG = g++
BUILD_DIR = ../build

GG_FLAGS = -c -m32 -fno-stack-protector -I ../lib 

all: $(BUILD_DIR)/disk.cc.o $(BUILD_DIR)/keyboard.cc.o

$(BUILD_DIR)/disk.cc.o: disk.cc
	$(GG) $(GG_FLAGS) $< -o $@	

$(BUILD_DIR)/keyboard.cc.o: keyboard.cc
	$(GG) $(GG_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/disk.cc.o $(BUILD_DIR)/keyboard.cc.o


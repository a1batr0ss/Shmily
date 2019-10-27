GG = g++
BUILD_DIR = ../build

GG_FLAGS = -c -m32 -fno-stack-protector -I ../lib 

all: $(BUILD_DIR)/memory.cc.o $(BUILD_DIR)/memory_manager.cc.o

$(BUILD_DIR)/memory.cc.o: memory.cc
	$(GG) $(GG_FLAGS) $< -o $@	

$(BUILD_DIR)/memory_manager.cc.o: manager.cc
	$(GG) $(GG_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/memory.cc.o $(BUILD_DIR)/memory_manger.cc.o


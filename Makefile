### Makefile for VideoCore IV compiler ###
SRC_DIR		:= $(CURDIR)/llvm9.0.1/llvm
BUILD_DIR	:= $(SRC_DIR)/build
BIN_DIR		:= $(BUILD_DIR)/bin

CMAKE		:= cmake
CMAKE_FLAGS	:= -DLLVM_TARGETS_TO_BUILD="X86"
CMAKE_FLAGS	+= -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD="VideoCore4"
CMAKE_FLAGS     += -DCMAKE_CXX_FLAGS="-DLLVM_ENABLE_DUMP -DDEBUG_REGISTER"
CMAKE_FLAGS	+= -DLLVM_ENABLE_PROJECTS="clang;lld"
#CMAKE_FLAGS	+= -DCMAKE_BUILD_TYPE=Debug
CMAKE_FLAGS	+= -DCMAKE_BUILD_TYPE=Release
CMAKE_FLAGS	+= -DLLVM_ENABLE_LLD=On
CMAKE_FLAGS	+= -DCMAKE_C_COMPILER=clang
CMAKE_FLAGS	+= -DCMAKE_CXX_COMPILER=clang++
CMAKE_FLAGS	+= -G Ninja

MAKE		:= ninja all

all: build

.PHONY: build
build: cmake
	[ -d $(BUILD_DIR) ] && \
	cd $(BUILD_DIR) && \
	$(MAKE) $(MAKE_FLAGS)

.PHONY: cmake
cmake: $(BUILD_DIR)/CMakeCache.txt

$(BUILD_DIR)/CMakeCache.txt:
	[ -d $(BUILD_DIR) ] || mkdir -p $(BUILD_DIR) && \
	cd $(BUILD_DIR) && \
	cmake $(CMAKE_FLAGS) $(SRC_DIR)

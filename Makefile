### Makefile for VideoCore IV compiler ###
SRC_DIR		:= $(CURDIR)/llvm8.0.0/llvm
BUILD_DIR	:= $(SRC_DIR)/build
BIN_DIR		:= $(BUILD_DIR)/bin

CMAKE		:= cmake
#CMAKE_FLAGS	:= -DLLVM_TARGETS_TO_BUILD="X86"
CMAKE_FLAGS	+= -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD="VideoCore4"
CMAKE_FLAGS     += -DCMAKE_CXX_FLAGS="-DLLVM_ENABLE_DUMP -DDEBUG_REGISTER"
CMAKE_FLAGS	+= -DLLVM_ENABLE_PROJECTS="clang;lld"
#CMAKE_FLAGS	+= -DCMAKE_BUILD_TYPE=Debug
CMAKE_FLAGS	+= -DCMAKE_BUILD_TYPE=Release
CMAKE_FLAGS	+= -G "Unix Makefiles"

MAKE		:= make
MAKE_FLAGS	:= -j`nproc` -l`nproc`

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

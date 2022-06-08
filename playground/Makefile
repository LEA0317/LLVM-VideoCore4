BIN_DIR 	:= ../build/bin
#INC_DIR	:= 

CLANG		:= $(BIN_DIR)/clang
CLANG_FLAGS	+= -Ofast
CLANG_FLAGS	+= -target videocore -mcpu=vc4
#CLANG_FLAGS	+= -flto=thin

LLC       := $(BIN_DIR)/llc
LLC_FLAGS += -O3
LLC_FLAGS += -march=videocore -mcpu=vc4
LLC_FLAGS += -relocation-model=static
LLC_FLAGS += -enable-ipra
#LLC_FLAGS += -show-mc-encoding
LLC_FLAGS += -time-passes
LLC_FLAGS += -filetype=asm
LLC_FLAGS += -show-mc-encoding

CPP_FILES := $(wildcard *.cpp)
LL_FILES  := $(subst .cpp,.ll,$(CPP_FILES))
S_FILES   := $(subst .cpp,.s,$(CPP_FILES))
TARGET = $(notdir $(CURDIR))

.PHONY: all
all: $(S_FILES)

%.ll: %.cpp
	$(CLANG) -emit-llvm -S -c $(CLANG_FLAGS) $^ -o $@ 2>&1 | tee  $(subst .ll,.cpp.ll.log,$@)

%.s: %.ll
	$(LLC) $(LLC_FLAGS) $^ -o $*.s  > $*.ll.s.log  2>&1

.PHONY: clean
clean:
	rm *.s *.log *.ll

.SECONDARY: $(foreach f,$(CPP_FILES),$(subst .cpp,.ll,$(f)))
.SECONDARY: $(foreach f,$(CPP_FILES),$(subst .cpp,.s,$(f)))

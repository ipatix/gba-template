####################
# main configuration
####################
PROGRAM   := helloworld

INC_DIR   := include
SRC_DIR   := src
BUILD_DIR := build

CFLAGS	  := -O2 -Wall -Wextra -flto -I $(INC_DIR) -mcpu=arm7tdmi -mthumb -mthumb-interwork
CXXFLAGS  := -O2 -Wall -Wextra -flto -I $(INC_DIR) -mcpu=arm7tdmi -mthumb -mthumb-interwork -fno-exceptions -fno-rtti -fpermissive
LDFLAGS   := -Wl,-Map=$(PROGRAM).map --specs=nano.specs --specs=nosys.specs

UART_DEV  := /dev/ttyUSB0

#########################
# game header information
#########################
TITLE		:= helloworld
GAME_CODE   := EAAE
MAKER_CODE  := 01
REVISION    := 0


######################
# libgba configuration
######################
LIBGBA     := libgba
LIBGBA_INC := $(LIBGBA)/include
LIBGBA_FLAGS := -I $(LIBGBA_INC)
LIBGBA_LDFLAGS := -L $(LIBGBA)/lib -lgba -nostartfiles $(LIBGBA)/gba_crt0.o -Wl,-T,$(LIBGBA)/gba_cart.ld
CFLAGS   := $(CFLAGS) $(LIBGBA_FLAGS)
CXXFLAGS := $(CXXFLAGS) $(LIBGBA_FLAGS)

###################
# compilation paths
###################
C_BUILD_DIR := $(BUILD_DIR)/c
C_SRCS := $(shell find src/ -type f -name '*.c')
C_OBJS := $(patsubst %.c,$(C_BUILD_DIR)/%.o,$(C_SRCS))

CXX_BUILD_DIR := $(BUILD_DIR)/cxx
CXX_SRCS := $(shell find src/ -type f -name '*.cpp')
CXX_OBJS := $(patsubst %.cpp,$(CXX_BUILD_DIR)/%.o,$(CXX_SRCS))

ASM_BUILD_DIR := $(BUILD_DIR)/asm
ASM_SRCS := $(shell find src/ -type f -name '*.s')
ASM_OBJS := $(patsubst %.s,$(ASM_BUILD_DIR)/%.o,$(ASM_SRCS))

OBJS := $(C_OBJS) $(CXX_OBJS) $(ASM_OBJS)

###########
# toolchain
###########
PREF    := arm-none-eabi-
AS      := $(PREF)as
CC      := $(PREF)gcc
CXX     := $(PREF)g++
LD      := $(CXX)
OBJCOPY := $(PREF)objcopy

###################
# compilation rules
###################
.PHONY: all clean flash devstat $(LIBGBA)
all: $(PROGRAM).gba

clean:
	rm -rf $(PROGRAM).gba $(PROGRAM).elf build/*
	make -C $(LIBGBA) clean

$(PROGRAM).gba: $(PROGRAM).elf
	$(OBJCOPY) -O binary $< $@
	gbafix $@ -t"$(TITLE)" -c$(GAME_CODE) -m$(MAKER_CODE) -r$(REVISION) -p

$(PROGRAM).elf: $(OBJS)
	$(LD) -o $@ $^ $(CXXFLAGS) $(LIBGBA_LDFLAGS) $(LDFLAGS)

$(C_BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS)

$(CXX_BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(ASM_BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) -o $@ $<

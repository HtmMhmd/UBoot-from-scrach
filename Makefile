# Simple U-Boot Makefile

# Architecture options
ARCH ?= arm  # Default to ARM, use "make ARCH=x86" for x86 build

# For QEMU ARM emulation, we need the cross-compiler
ARM_CC = arm-none-eabi-gcc
ARM_LD = arm-none-eabi-ld
ARM_OBJCOPY = arm-none-eabi-objcopy
ARM_OBJDUMP = arm-none-eabi-objdump

# Check if ARM cross-compiler is available
HAS_ARM_COMPILER := $(shell which $(ARM_CC) 2>/dev/null)

# Set appropriate toolchain and flags based on architecture and availability
ifeq ($(ARCH),arm)
  ifdef HAS_ARM_COMPILER
    # Use ARM toolchain
    CC = $(ARM_CC)
    LD = $(ARM_LD)
    OBJCOPY = $(ARM_OBJCOPY)
    OBJDUMP = $(ARM_OBJDUMP)
    
    # ARM-specific flags
    ARCH_FLAGS = -mcpu=cortex-a9
    LDSCRIPT = u-boot-arm.lds
    ASM_SRCS = src/arm_startup.S
    $(info Building for ARM architecture with cross-compiler)
  else
    $(error ARM cross-compiler not found. Install with: sudo apt install gcc-arm-none-eabi)
  endif
else ifeq ($(ARCH),x86)
  # Use native compiler for x86
  CC = gcc
  LD = ld
  OBJCOPY = objcopy
  OBJDUMP = objdump
  
  # x86 specific flags
  ARCH_FLAGS = -m32 -march=i686
  LDSCRIPT = u-boot-x86.lds
  ASM_SRCS = src/x86_startup.S
  $(info Building for x86 architecture)
else
  $(error Unsupported architecture: $(ARCH). Use ARCH=arm or ARCH=x86)
endif

# Common flags for both architectures
CFLAGS = -Wall -Wextra -I./include -nostdlib -ffreestanding -O2 -g $(ARCH_FLAGS)
LDFLAGS = -nostdlib -T $(LDSCRIPT)

SRCS = src/main.c \
       src/console.c \
       src/commands.c \
       src/memory.c \
       board/generic/board.c

OBJS = $(SRCS:.c=.o)
ASM_OBJS = $(ASM_SRCS:.S=.o)

all: u-boot.bin

u-boot.bin: u-boot.elf
    $(OBJCOPY) -O binary $< $@
    @echo "Dumping ELF information for debugging:"
    $(OBJDUMP) -d $< > u-boot.dis
    $(OBJDUMP) -h $<

u-boot.elf: $(OBJS) $(ASM_OBJS) $(LDSCRIPT)
    $(LD) $(LDFLAGS) -o $@ $(OBJS) $(ASM_OBJS)

%.o: %.c
    $(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
    $(CC) $(CFLAGS) -c $< -o $@

clean:
    rm -f $(OBJS) $(ASM_OBJS) u-boot.elf u-boot.bin u-boot.dis

run: u-boot.bin
    ./run_qemu.sh

run_x86: u-boot.bin
    ./run_qemu_x86.sh

# Target to create linker scripts from templates if they don't exist
$(LDSCRIPT):
    @if [ ! -f $@ ]; then \
        echo "Creating $@ from template..."; \
        cp u-boot.lds $@; \
    fi

.PHONY: all clean run run_x86 install_deps
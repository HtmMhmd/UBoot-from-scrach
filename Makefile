# Simple U-Boot Makefile

# For QEMU ARM emulation, we need the cross-compiler
ARM_CC = arm-none-eabi-gcc
ARM_LD = arm-none-eabi-ld
ARM_OBJCOPY = arm-none-eabi-objcopy
ARM_OBJDUMP = arm-none-eabi-objdump

# Check if ARM cross-compiler is available
HAS_ARM_COMPILER := $(shell which $(ARM_CC) 2>/dev/null)

# Set appropriate toolchain and flags based on availability
ifdef HAS_ARM_COMPILER
  # Use ARM toolchain
  CC = $(ARM_CC)
  LD = $(ARM_LD)
  OBJCOPY = $(ARM_OBJCOPY)
  OBJDUMP = $(ARM_OBJDUMP)
  
  # ARM-specific flags
  ARCH_FLAGS = -march=armv7-a -mcpu=cortex-a9
  $(info Using ARM cross-compiler)
else
  # Fall back to native compiler for educational build
  CC = gcc
  LD = ld
  OBJCOPY = objcopy
  OBJDUMP = objdump
  
  # Remove ARM-specific flags for native build
  ARCH_FLAGS = 
  $(info ARM cross-compiler not found, using native compiler (for educational purposes only))
  $(info To install ARM toolchain: sudo apt install gcc-arm-none-eabi)
endif

# Common flags for both toolchains
CFLAGS = -Wall -Wextra -I./include -nostdlib -ffreestanding -O2 -g $(ARCH_FLAGS)
LDFLAGS = -nostdlib -T u-boot.lds

SRCS = src/main.c \
       src/console.c \
       src/commands.c \
       src/memory.c \
       board/generic/board.c

ifdef HAS_ARM_COMPILER
  # Add ARM assembly startup when using ARM compiler
  SRCS += src/startup.S
endif

OBJS = $(SRCS:.c=.o) 
ifdef HAS_ARM_COMPILER
  OBJS += src/startup.o
endif

all: u-boot.bin

u-boot.bin: u-boot.elf
	$(OBJCOPY) -O binary $< $@
	@echo "Dumping ELF information for debugging:"
	$(OBJDUMP) -d $< > u-boot.dis
	$(OBJDUMP) -h $<

u-boot.elf: $(OBJS) u-boot.lds
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) u-boot.elf u-boot.bin u-boot.dis

debug:
	gdb-multiarch -ex "target remote localhost:1234" u-boot.elf

run: u-boot.bin
	./run_qemu.sh

.PHONY: all clean debug run install_deps

# Target to install dependencies
install_deps:
	@echo "Installing required dependencies..."
	@echo "For Ubuntu/Debian:"
	@echo "  sudo apt-get update"
	@echo "  sudo apt-get install gcc-arm-none-eabi qemu-system-arm"
	@echo "For Fedora:"
	@echo "  sudo dnf install arm-none-eabi-gcc qemu"

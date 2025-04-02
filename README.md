# Simple U-Boot Implementation

This is a minimalistic implementation of U-Boot bootloader, designed for educational purposes.

## Features
- Basic hardware initialization
- Simple command line interface
- Memory management
- Boot process for loading operating systems

## Understanding the Code

### Bootloader Overview
U-Boot (Universal Bootloader) is responsible for initializing hardware and loading an operating system. This implementation demonstrates the core concepts of a bootloader in a simplified manner.

The bootloader's main tasks include:
- Setting up hardware (clocks, memory controllers)
- Providing a user interface to configure boot parameters
- Loading an operating system kernel into memory
- Transferring control to the loaded OS

Example boot sequence in `main.c`:
```c
int main(void)
{
    /* Early board initialization */
    board_early_init();
    
    /* Initialize console */
    console_init();
    
    /* Print banner */
    print_banner();
    
    /* Board initialization */
    ret = board_init();
    
    /* Memory initialization */
    ret = mem_init();
    
    /* Command initialization */
    cmd_init();
    
    /* Main command loop */
    while (1) {
        printf("u-boot> ");
        if (gets(cmd_buffer, sizeof(cmd_buffer)) > 0) {
            cmd_process(cmd_buffer);
        }
    }
}
```

### Boot Process
1. **Early Initialization**: The bootloader starts by performing early hardware setup (clocks, basic I/O)

   ```c
   int board_early_init(void)
   {
       /* Disable interrupts */
       /* Configure clock */
       /* Basic hardware setup */
       return 0;
   }
   ```

2. **Console Setup**: Initializes UART for input/output communication

   ```c
   void console_init(void)
   {
       /* Initialize UART hardware */
       board_uart_init();
   }
   ```

3. **Hardware Initialization**: Completes full board-specific hardware setup

   ```c
   int board_init(void)
   {
       /* Initialize board-specific hardware */
       printf("Initializing %s\n", BOARD_NAME);
       return 0;
   }
   ```

4. **Memory Setup**: Initializes memory controllers and sets up a simple memory allocator

   ```c
   int mem_init(void)
   {
       mem_start = RAM_START + 0x10000; /* Reserve memory for bootloader */
       mem_end = RAM_START + RAM_SIZE;
       mem_current = mem_start;
       
       printf("Memory initialized: %dMB available\n", 
              (mem_end - mem_current) / (1024 * 1024));
       return 0;
   }
   ```

5. **Command Interface**: Provides a command-line interface for user interaction
6. **OS Loading**: (Simulated) Capability to load and execute an operating system

### Command System
The command processor allows users to interact with the bootloader through a simple shell:
- Commands are registered in a command table with name, help text, and function pointer
- Built-in commands include: help, version, reset, and boot
- The command parser tokenizes input and dispatches to the appropriate handler

Example command definition:
```c
static const cmd_t builtin_cmds[] = {
    {"help", "Show available commands", cmd_help},
    {"version", "Show bootloader version", cmd_version},
    {"reset", "Reset the board", cmd_reset},
    {"boot", "Boot the operating system", cmd_boot},
    {NULL, NULL, NULL}  /* Terminator */
};
```

Command processing workflow:
```c
int cmd_process(char *cmdline)
{
    /* Parse arguments */
    argv[argc++] = p;
    while (*p) {
        if (*p == ' ') {
            *p++ = '\0';
            /* Skip spaces */
            while (*p && *p == ' ')
                p++;
            
            if (*p)
                argv[argc++] = p;
        } else {
            p++;
        }
    }
    
    /* Look up and execute command */
    cmd = cmd_lookup(argv[0]);
    if (cmd != NULL)
        return cmd->func(argc, argv);
}
```

Example command-line interaction:
```
u-boot> help
Available commands:
------------------
help       - Show available commands
version    - Show bootloader version
reset      - Reset the board
boot       - Boot the operating system

u-boot> version
Simple U-Boot version 1.0
Board: Generic Board

u-boot> boot
Booting operating system...
Not implemented in this simple version
```

### Memory Management
This implementation uses a simple "bump allocator" for memory management:
- Memory is allocated sequentially without complex tracking
- `malloc()` returns memory but `free()` is not implemented (common in bootloaders)
- Basic memory utilities (`memcpy`, `memset`) are provided

Memory allocation example:
```c
void *malloc(size_t size)
{
    addr_t addr;
    
    /* Align to 8 bytes */
    size = (size + 7) & ~7;
    
    if (mem_current + size >= mem_end)
        return NULL;
    
    addr = mem_current;
    mem_current += size;
    
    return (void *)addr;
}
```

Memory mapping (defined in u-boot.lds):
```
SECTIONS
{
    /* Start at 0x80000000 */
    . = 0x80000000;

    .text : {
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }
}
```

### Console I/O
The console system provides:
- Character-based input/output through UART
- Line editing with backspace support
- A simple `printf` implementation with basic format specifiers (%c, %s, %d, %x)

UART register access example:
```c
/* UART registers structure */
typedef struct {
    volatile uint32_t data;    /* Data register */
    volatile uint32_t status;  /* Status register */
    volatile uint32_t control; /* Control register */
} uart_regs_t;

#define UART ((uart_regs_t*)UART_BASE)

void putc(char c)
{
    /* Wait until TX buffer is not full */
    while (UART->status & UART_STATUS_TX_FULL)
        ;

    /* Send character */
    UART->data = c;
}
```

Line input example:
```c
int gets(char *buffer, int size)
{
    int i = 0;
    char c;

    while (1) {
        c = getc();
        
        /* Handle backspace */
        if (c == '\b' || c == 0x7F) {
            if (i > 0) {
                i--;
                puts("\b \b");  /* Erase character on terminal */
            }
            continue;
        }
        
        /* Check for end of line */
        if (c == '\r' || c == '\n') {
            buffer[i] = '\0';
            putc('\n');
            return i;
        }
        
        /* Store character if buffer has space */
        if (i < size - 1)
            buffer[i++] = c;
    }
}
```

### Hardware Abstraction
The board interface abstracts hardware-specific operations:
- Early initialization (before main peripherals are ready)
- Full board initialization
- UART setup for console I/O
- Memory controller configuration
- System reset functionality

Example of a board reset function:
```c
void board_reset(void)
{
    /* Write to the reset control register to trigger a reset */
    *((volatile uint32_t *)RESET_CONTROL_REG) = 1;
    
    /* Should not reach here */
    while (1) {}
}
```

### Build System
The Makefile provides:
- Compilation of C source files with appropriate embedded system flags
- Linking with a custom linker script for memory layout
- Generation of binary image suitable for flashing

Key build flags:
```makefile
CFLAGS = -Wall -Wextra -I./include -nostdlib -ffreestanding -O2 -g
LDFLAGS = -nostdlib -T u-boot.lds
```

Build commands:
```bash
# Complete build
$ make

# Clean build artifacts
$ make clean

# Compile a specific file
$ make src/main.o

# Generate binary image
$ make u-boot.bin
```

## Understanding Make

### What is Make?
Make is a build automation tool that automatically builds executable programs and libraries from source code. It determines which pieces of a program need to be recompiled and issues the commands to recompile them when files are modified.

### Key Make Concepts

1. **Targets**: The files to be built (like `u-boot.bin` or `u-boot.elf`)
2. **Dependencies**: Files that targets depend on (like source files or other targets)
3. **Rules**: Instructions to build a target from its dependencies
4. **Variables**: Store values like compiler flags or file lists
5. **Phony Targets**: Special targets that don't represent files (like "clean" or "all")

### U-Boot Makefile Explained

Here's the breakdown of our U-Boot Makefile:

```makefile
CC = gcc               # Compiler to use
LD = ld                # Linker to use
OBJCOPY = objcopy      # Object copy utility

# Compilation flags
CFLAGS = -Wall -Wextra -I./include -nostdlib -ffreestanding -O2 -g
LDFLAGS = -nostdlib -T u-boot.lds

# Source files
SRCS = src/main.c \
       src/console.c \
       src/commands.c \
       src/memory.c \
       board/generic/board.c

# Object files (generated from source files)
OBJS = $(SRCS:.c=.o)

# Default target (what happens when you run "make" with no arguments)
all: u-boot.bin

# Create binary image from ELF file
u-boot.bin: u-boot.elf
	$(OBJCOPY) -O binary $< $@

# Link object files into ELF file
u-boot.elf: $(OBJS) u-boot.lds
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

# Compile C files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Remove all build artifacts
clean:
	rm -f $(OBJS) u-boot.elf u-boot.bin

# Mark targets that don't represent actual files
.PHONY: all clean
```

### Common Make Commands for U-Boot

```bash
# Build everything (default)
$ make

# Clean all build artifacts
$ make clean

# Build a specific file
$ make src/main.o

# Rebuild everything from scratch
$ make clean all

# Show the commands being executed (verbose mode)
$ make V=1

# Use a specific compiler
$ make CC=arm-none-eabi-gcc
```

### Customizing the U-Boot Build

To add new source files to the build:

1. Add the file path to the `SRCS` variable in the Makefile:
```makefile
SRCS = src/main.c \
       src/console.c \
       src/commands.c \
       src/memory.c \
       board/generic/board.c \
       src/your_new_file.c
```

To add a new board configuration:

1. Create board directory and files as described in "Extending the Bootloader"
2. Update the Makefile to use your board:
```makefile
SRCS = src/main.c \
       src/console.c \
       src/commands.c \
       src/memory.c \
       board/your_board/board.c
```

To configure for cross-compilation for a specific architecture:

```bash
# Example for ARM cross-compilation
$ make CC=arm-none-eabi-gcc LD=arm-none-eabi-ld OBJCOPY=arm-none-eabi-objcopy
```

### Advanced Make Features for U-Boot

Add these to your Makefile to enable more features:

```makefile
# Generate dependency files automatically
%.d: %.c
	$(CC) $(CFLAGS) -MM -MF $@ $<

# Include generated dependencies
-include $(SRCS:.c=.d)

# Build with debug information
debug: CFLAGS += -DDEBUG -g3
debug: all

# Create a specific board configuration
board_raspi: CFLAGS += -DBOARD_RASPI
board_raspi: SRCS = src/main.c src/console.c src/commands.c src/memory.c board/raspi/board.c
board_raspi: all
```

## Running U-Boot

### Running on Real Hardware

1. **Build the bootloader**:
   ```bash
   $ make
   ```

2. **Flash the bootloader to your target device**:
   For most embedded systems, you'll need to use a hardware programmer or bootloader to flash U-Boot:
   
   Using OpenOCD (Open On-Chip Debugger):
   ```bash
   $ openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program u-boot.bin 0x08000000 verify reset exit"
   ```
   
   Using STM32 Flash Loader (for STM32 microcontrollers):
   ```bash
   $ STM32_Programmer_CLI -c port=SWD -w u-boot.bin 0x08000000 -v -rst
   ```

3. **Connect to the serial console**:
   U-Boot communicates via serial UART. Connect a USB-to-Serial adapter to your board's UART pins, then use a terminal program:
   
   ```bash
   $ screen /dev/ttyUSB0 115200
   ```
   
   Or using minicom:
   ```bash
   $ minicom -D /dev/ttyUSB0 -b 115200
   ```

4. **Interacting with U-Boot**:
   After reset, you should see the U-Boot banner and command prompt:
   ```
   *********************************************
   *                                           *
   *            Simple U-Boot v1.0             *
   *                                           *
   *********************************************
   
   Starting initialization...
   Initializing Generic Board
   Memory initialized: 15MB available
   Command system initialized with 4 commands
   Initialization complete. Starting command interface.
   Type 'help' for available commands.

   u-boot> 
   ```

### Running in QEMU Emulation

QEMU provides an excellent way to test U-Boot without actual hardware. Here's a comprehensive guide to using QEMU with our simple U-Boot implementation:

#### Basic QEMU Setup

1. **Install QEMU**:
   ```bash
   # For Debian/Ubuntu
   $ sudo apt install qemu-system-arm qemu-system-x86 qemu-utils
   
   # For Fedora
   $ sudo dnf install qemu qemu-system-arm qemu-system-x86
   
   # For macOS (using Homebrew)
   $ brew install qemu
   ```

2. **Using the run_qemu.sh script** (recommended):
   ```bash
   $ chmod +x run_qemu.sh
   $ ./run_qemu.sh
   ```

3. **Fixing PulseAudio errors**:
   If you see errors like:
   ```
   pulseaudio: set_sink_input_volume() failed
   pulseaudio: Reason: Invalid argument
   pulseaudio: set_sink_input_mute() failed
   pulseaudio: Reason: Invalid argument
   ```
   
   Use the following command to disable audio in QEMU:
   ```bash
   $ qemu-system-arm -M vexpress-a9 -nographic -kernel u-boot.bin -audiodev none,id=audio1
   ```

#### QEMU Machine Options

Different hardware platforms can be emulated:

```bash
# ARM Versatile Express board
$ qemu-system-arm -M vexpress-a9 -nographic -kernel u-boot.bin

# ARM Virtual Development Board
$ qemu-system-arm -M virt -nographic -kernel u-boot.bin

# QEMU ARM Versatile PB
$ qemu-system-arm -M versatilepb -nographic -kernel u-boot.bin

# For x86 systems (if you adapt the code for x86)
$ qemu-system-i386 -nographic -kernel u-boot.bin
```

#### Adding Storage Devices

To test storage functionality:

1. **Create a disk image**:
   ```bash
   $ qemu-img create -f raw disk.img 100M
   ```

2. **Run with the disk image**:
   ```bash
   $ qemu-system-arm -M vexpress-a9 -nographic -kernel u-boot.bin \
     -drive file=disk.img,format=raw,if=sd
   ```

#### Adding Network Support

For testing network functionality:

```bash
$ qemu-system-arm -M vexpress-a9 -nographic -kernel u-boot.bin \
  -netdev user,id=mynet -device virtio-net-device,netdev=mynet
```

#### QEMU with GDB Debugging

For debugging U-Boot in QEMU:

1. **Start QEMU with GDB server**:
   ```bash
   $ qemu-system-arm -M vexpress-a9 -nographic -kernel u-boot.bin -S -s
   ```
   
   The `-S` option freezes the CPU at startup, and `-s` is a shorthand for `-gdb tcp::1234`

2. **Connect GDB in another terminal**:
   ```bash
   $ arm-none-eabi-gdb u-boot.elf
   (gdb) target remote localhost:1234
   (gdb) break main
   (gdb) continue
   ```

#### QEMU Control Keys

While running QEMU in nographic mode:
- Press `Ctrl+A` then `x` to exit QEMU
- Press `Ctrl+A` then `h` to see help
- Press `Ctrl+A` then `c` to toggle between the console and QEMU monitor

#### Passing Kernel and Device Tree to U-Boot

If you want to test the full boot process:

1. **Create a directory for test files**:
   ```bash
   $ mkdir -p /home/hatem/UBoot\ from\ scrach/qemu_test
   ```

2. **Download a test kernel and device tree**:
   ```bash
   $ cd /home/hatem/UBoot\ from\ scrach/qemu_test
   $ wget https://github.com/u-boot/u-boot/raw/master/doc/README.arm-versatile
   ```

3. **Run QEMU with these files**:
   ```bash
   $ qemu-system-arm -M vexpress-a9 -nographic -kernel u-boot.bin \
     -drive file=disk.img,format=raw,if=sd \
     -dtb qemu_test/vexpress-v2p-ca9.dtb \
     -initrd qemu_test/ramdisk.img \
     -append "console=ttyAMA0"
   ```

#### Script for Running U-Boot in QEMU

Create a script to simplify launching QEMU:

```bash
#!/bin/bash
# /home/hatem/UBoot from scrach/run_qemu.sh

MACHINE=vexpress-a9
UBOOT=u-boot.bin

qemu-system-arm -M ${MACHINE} \
  -nographic \
  -kernel ${UBOOT} \
  -drive file=disk.img,format=raw,if=sd

# Make the script executable
# chmod +x run_qemu.sh
```

Run it with:
```bash
$ ./run_qemu.sh
```

### Running with GDB for Debugging

1. **Build with debug symbols**:
   ```bash
   $ make CFLAGS="-Wall -Wextra -I./include -nostdlib -ffreestanding -O0 -g"
   ```

2. **Start GDB server** (using OpenOCD as an example):
   ```bash
   $ openocd -f interface/stlink.cfg -f target/stm32f4x.cfg
   ```

3. **Connect GDB to the target**:
   ```bash
   $ arm-none-eabi-gdb u-boot.elf
   (gdb) target remote localhost:3333
   (gdb) monitor reset halt
   (gdb) load
   (gdb) break main
   (gdb) continue
   ```

### Common U-Boot Boot Sequence

Here's a typical U-Boot boot sequence:

1. **Power on** - Hardware initialization begins
2. **U-Boot SPL** - (if used) First-stage loader sets up minimal hardware
3. **U-Boot proper** - Main bootloader runs
4. **Environment setup** - Loads environment variables
5. **Autoboot** - Automatically boots the default OS after a timeout
6. **Manual interaction** - User can interrupt autoboot to enter commands
7. **Boot OS** - Loads and jumps to the operating system

### Common U-Boot Commands

Once at the U-Boot prompt, you can interact with various commands:

```
u-boot> help                # Show available commands
u-boot> version             # Show U-Boot version
u-boot> reset               # Reset the board
u-boot> boot                # Boot the default OS
```

In a full U-Boot implementation, you would also have:

```
u-boot> printenv            # Print environment variables
u-boot> setenv name value   # Set environment variable
u-boot> saveenv             # Save environment to storage
u-boot> load mmc 0:1 ${loadaddr} uImage  # Load kernel from MMC
u-boot> bootm ${loadaddr}   # Boot kernel image at loadaddr
```

## Code Structure

- `include/`: Header files defining interfaces and constants
  - `types.h`: Basic type definitions
  - `config.h`: Hardware and bootloader configuration
  - `board.h`: Board-specific operation interfaces
  - `console.h`: Console I/O functions
  - `commands.h`: Command processing interfaces
  - `memory.h`: Memory management functions

- `src/`: Core bootloader implementation
  - `main.c`: Entry point and main loop
  - `console.c`: Console I/O implementation
  - `commands.c`: Command processing and built-in commands
  - `memory.c`: Simple memory allocator

- `board/generic/`: Board-specific implementations
  - `board.c`: Generic board implementation

- `u-boot.lds`: Linker script defining memory layout

## Building
```bash
# Clone the repository
$ git clone https://example.com/simple-uboot.git
$ cd simple-uboot

# Build the bootloader
$ make

# Output files
$ ls -l u-boot.bin u-boot.elf
```

## Usage
Flash the compiled bootloader to your target device following your hardware's programming instructions.

Example flashing commands (actual commands depend on your hardware):
```bash
# For platforms with OpenOCD support
$ openocd -f board/your_board.cfg -c "program u-boot.bin 0x80000000 verify reset exit"

# Using proprietary flash tools
$ vendor-flash-tool --device=YOUR_BOARD --file=u-boot.bin --addr=0x80000000
```

## Extending the Bootloader

To adapt this bootloader for a real device:

1. Create a new board directory under `board/`:
```bash
$ mkdir -p board/your_board_name
$ touch board/your_board_name/board.c
```

2. Implement board-specific functions in `board.c`:
```c
#include "../../include/types.h"
#include "../../include/config.h"
#include "../../include/board.h"

int board_early_init(void)
{
    /* Your custom early initialization code */
    /* E.g., setting up PLLs, clocks, etc. */
    return 0;
}

int board_uart_init(void)
{
    /* Initialize your specific UART hardware */
    /* E.g., setting registers for baud rate, data bits, etc. */
    return 0;
}

/* Implement other board-specific functions */
```

3. Update `config.h` with correct hardware addresses and parameters:
```c
/* Board configurations */
#define BOARD_NAME          "Your Board Name"
#define CPU_CLOCK_HZ        100000000   /* 100 MHz */
#define RAM_START           0x20000000  /* Your RAM start address */
#define RAM_SIZE            0x02000000  /* 32 MB or your actual RAM size */

/* Console configurations */
#define UART_BASE           0x40001000  /* Your UART base address */
```

4. Modify the linker script to match your hardware's memory map:
```
SECTIONS
{
    /* Your actual memory start address */
    . = 0x20000000;

    .text : {
        *(.text)
    }
    /* Other sections */
}
```

5. Add device-specific drivers for storage, networking, etc.:
```c
/* Example storage driver interface */
int mmc_init(void);
int mmc_read(void *buf, int start_sector, int count);
int mmc_write(const void *buf, int start_sector, int count);
```

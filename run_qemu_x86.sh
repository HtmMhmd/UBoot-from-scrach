#!/bin/bash
# Script to run U-Boot in QEMU with proper settings for x86 architecture

UBOOT_BIN="u-boot.bin"
MACHINE="q35"  # Common machine type for x86

# Check if binary exists
if [ ! -f "$UBOOT_BIN" ]; then
    echo "Error: $UBOOT_BIN not found. Did you build U-Boot for x86?"
    echo "To build U-Boot for x86, run: make <x86_defconfig> && make"
    exit 1
fi

# Show what we're doing
echo "Starting QEMU with U-Boot for x86..."
echo "Machine: ${MACHINE}"
echo "Kernel: ${UBOOT_BIN}"

# Check for QEMU
if ! command -v qemu-system-i386 &> /dev/null; then
    echo "Error: qemu-system-i386 not found. Please install QEMU."
    echo "  For Ubuntu/Debian: sudo apt-get install qemu-system-x86"
    echo "  For Fedora: sudo dnf install qemu"
    exit 1
fi

# Run QEMU for x86
qemu-system-i386 \
  -M ${MACHINE} \
  -nographic \
  -bios ${UBOOT_BIN} \
  -no-reboot \
  -d guest_errors,unimp \
  -audiodev none,id=audio1

# Exit with the exit code from QEMU
EXIT_CODE=$?
# Exit with the exit code from QEMU
EXIT_CODE=$?
if [ $EXIT_CODE -ne 0 ]; then
    echo "QEMU exited with error code: $EXIT_CODE"
fi
exit $EXIT_CODE
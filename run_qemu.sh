#!/bin/bash
# Script to run U-Boot in QEMU with proper settings

UBOOT_BIN="u-boot.bin"
MACHINE="vexpress-a9"

# Check if binary exists
if [ ! -f "$UBOOT_BIN" ]; then
    echo "Error: $UBOOT_BIN not found. Did you run 'make' first?"
    echo "To build U-Boot, run: make"
    echo "To install dependencies: make install_deps"
    exit 1
fi

# Show what we're doing
echo "Starting QEMU with U-Boot..."
echo "Machine: ${MACHINE}"
echo "Kernel: ${UBOOT_BIN}"

# Check for QEMU
if ! command -v qemu-system-arm &> /dev/null; then
    echo "Error: qemu-system-arm not found. Please install QEMU."
    echo "  For Ubuntu/Debian: sudo apt-get install qemu-system-arm"
    echo "  For Fedora: sudo dnf install qemu"
    exit 1
fi

# Run QEMU with better debug settings
qemu-system-arm \
  -M ${MACHINE} \
  -nographic \
  -kernel ${UBOOT_BIN} \
  -serial stdio \
  -no-reboot \
  -d guest_errors,unimp \
  -audiodev none,id=audio1

# Exit with the exit code from QEMU
EXIT_CODE=$?
if [ $EXIT_CODE -ne 0 ]; then
    echo "QEMU exited with error code: $EXIT_CODE"
fi
exit $EXIT_CODE

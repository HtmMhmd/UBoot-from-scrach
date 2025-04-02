#!/bin/bash
# Script to run U-Boot in QEMU with proper settings

UBOOT_BIN="u-boot.bin"
MACHINE="vexpress-a9"

# Check if binary exists
if [ ! -f "$UBOOT_BIN" ]; then
    echo "Error: $UBOOT_BIN not found. Run 'make' first."
    exit 1
fi

# Show what we're doing
echo "Starting QEMU with U-Boot..."
echo "Machine: ${MACHINE}"
echo "Kernel: ${UBOOT_BIN}"

# Run QEMU with improved configuration
qemu-system-arm \
  -M ${MACHINE} \
  -kernel ${UBOOT_BIN} \
  -nographic \
  -no-reboot \
  -nodefaults \
  -serial mon:stdio \
  -d guest_errors,unimp \
  -audiodev none,id=1

# Exit with the exit code from QEMU
EXIT_CODE=$?
if [ $EXIT_CODE -ne 0 ]; then
    echo "QEMU exited with error code: $EXIT_CODE"
fi
exit $EXIT_CODE

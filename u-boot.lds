/* U-Boot linker script for QEMU vexpress-a9 */

/* Choose entry point based on architecture */
#if defined(__arm__)
ENTRY(_start)
#else
ENTRY(_start)
#endif

SECTIONS
{
    /* QEMU vexpress-a9 loads at 0x60000000 */
    . = 0x60000000;

    .text : {
        _start = .;
        /* Add ARM vector table and entry point for QEMU */
        KEEP(*(.vector_table))
        KEEP(*(.startup))
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        _bss_start = .;
        *(.bss)
        _bss_end = .;
    }
}

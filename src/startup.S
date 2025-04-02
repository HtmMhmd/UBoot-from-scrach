/* ARM assembly startup code for QEMU compatibility */
.section .startup, "ax"
.global _start

_start:
    /* Setup stack pointer */
    ldr sp, =0x67000000

    /* Clear BSS section */
    ldr r0, =_bss_start
    ldr r1, =_bss_end
    mov r2, #0
1:
    cmp r0, r1
    strlt r2, [r0], #4
    blt 1b

    /* Jump to C code */
    bl main

    /* Should not return, but if it does */
    b .

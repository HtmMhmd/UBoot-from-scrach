/* ARM Startup Code */
.section .vector_table
.global _start

_start:
    /* Vector table */
    b     reset_handler    /* Reset */
    b     hang             /* Undefined instruction */
    b     hang             /* Software interrupt */
    b     hang             /* Prefetch abort */
    b     hang             /* Data abort */
    b     hang             /* Reserved */
    b     hang             /* IRQ */
    b     hang             /* FIQ */

reset_handler:
    /* Set up stack pointer */
    ldr   sp, =0x60100000  /* Use RAM address for stack */
    
    /* Call C code */
    bl    main
    
    /* Should not return, but if it does */
hang:
    b     hang

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "types.h"

/* Board configurations */
#define BOARD_NAME          "Generic Board"
#define CPU_CLOCK_HZ        50000000    /* 50 MHz */
#define RAM_START           0x60000000  /* QEMU vexpress-a9 RAM start */
#define RAM_SIZE            0x40000000  /* 1 GB - QEMU vexpress-a9 has more RAM */

/* Console configurations */
/* QEMU vexpress-a9 PL011 UART0 address */
#define UART_BASE           0x10009000
#define UART_BAUD_RATE      115200
#define MAX_CMD_LENGTH      64
#define MAX_ARGS            16

/* Environment configurations */
#define ENV_SIZE            0x1000      /* 4 KB */

/* Boot configurations */
#define KERNEL_LOAD_ADDR    0x60008000
#define INITRD_LOAD_ADDR    0x60800000
#define FDT_LOAD_ADDR       0x60F00000

#endif /* _CONFIG_H_ */

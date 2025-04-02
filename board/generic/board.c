#include "../../include/types.h"
#include "../../include/config.h"
#include "../../include/board.h"
#include "../../include/console.h"

/* Simple simulation of hardware registers */
#define RESET_CONTROL_REG 0x20000000

/* The first thing that will run - make it more verbose */
int board_early_init(void)
{
    /* Print directly to UART register for earliest possible output */
    volatile uint32_t *uart_data = (volatile uint32_t *)UART_BASE;
    const char *msg = "U-Boot early init\r\n";
    
    while (*msg) {
        *uart_data = *msg++;
        /* Simple delay */
        for (volatile int i = 0; i < 10000; i++);
    }
    
    return 0;
}

int board_init(void)
{
    /* Initialize board-specific hardware */
    printf("Initializing %s\n", BOARD_NAME);
    return 0;
}

int board_uart_init(void)
{
    /* For QEMU, just ensure we're accessing the right UART address */
    volatile uint32_t *uart_ctrl = (volatile uint32_t *)(UART_BASE + 0x30);
    /* Enable UART (simplified for QEMU) */
    *uart_ctrl = 0x301;
    
    return 0;
}

int board_mem_init(void)
{
    /* Initialize memory controllers - not needed for QEMU */
    return 0;
}

void board_reset(void)
{
    printf("Resetting system - not implemented in QEMU\n");
    /* In QEMU, we can just halt */
    while (1) {}
}

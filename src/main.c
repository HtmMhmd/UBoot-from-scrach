#include "../include/types.h"
#include "../include/config.h"
#include "../include/board.h"
#include "../include/console.h"
#include "../include/commands.h"
#include "../include/memory.h"

/* For non-ARM builds, define a simple entry point */
#ifndef __arm__
void _start(void) {
    /* Call main directly */
    main();
    
    /* Should not return, but if it does */
    while(1) {}
}
#endif

static void print_banner(void)
{
    puts("\n\n");
    puts("*********************************************\n");
    puts("*                                           *\n");
    puts("*            Simple U-Boot v1.0             *\n");
    puts("*                                           *\n");
    puts("*********************************************\n");
    puts("\n");
}

int main(void)
{
    char cmd_buffer[MAX_CMD_LENGTH];
    int ret;

    /* Early board initialization */
    board_early_init();
    
    /* Initialize console */
    console_init();
    
    /* Print banner */
    print_banner();
    
    printf("Starting initialization...\n");
    
    /* Board initialization */
    ret = board_init();
    if (ret != 0) {
        printf("Board initialization failed: %d\n", ret);
        return ret;
    }
    
    /* Memory initialization */
    ret = mem_init();
    if (ret != 0) {
        printf("Memory initialization failed: %d\n", ret);
        return ret;
    }
    
    /* Command initialization */
    cmd_init();
    
    printf("Initialization complete. Starting command interface.\n");
    printf("Type 'help' for available commands.\n\n");
    
    /* Main command loop */
    while (1) {
        printf("u-boot> ");
        if (gets(cmd_buffer, sizeof(cmd_buffer)) > 0) {
            cmd_process(cmd_buffer);
        }
    }

    /* Never reached */
    return 0;
}

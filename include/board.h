#ifndef _BOARD_H_
#define _BOARD_H_

#include "types.h"

/* Board initialization function */
int board_init(void);

/* Early hardware initialization */
int board_early_init(void);

/* UART initialization */
int board_uart_init(void);

/* Memory initialization */
int board_mem_init(void);

/* Reset board */
void board_reset(void);

#endif /* _BOARD_H_ */

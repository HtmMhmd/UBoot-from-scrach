#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "types.h"

/* Initialize console */
void console_init(void);

/* Print a character */
void putc(char c);

/* Print a string */
void puts(const char *str);

/* Print a formatted string */
void printf(const char *fmt, ...);

/* Read a character */
int getc(void);

/* Read a line */
int gets(char *buffer, int size);

#endif /* _CONSOLE_H_ */

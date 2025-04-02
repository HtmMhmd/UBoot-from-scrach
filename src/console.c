#include "../include/types.h"
#include "../include/config.h"
#include "../include/console.h"
#include "../include/board.h"

/* UART registers structure */
typedef struct {
    volatile uint32_t data;    /* Data register */
    volatile uint32_t status;  /* Status register */
    volatile uint32_t control; /* Control register */
} uart_regs_t;

#define UART ((uart_regs_t*)UART_BASE)

/* UART Status register bits */
#define UART_STATUS_TX_FULL 0x01
#define UART_STATUS_RX_EMPTY 0x02

void console_init(void)
{
    /* Initialize UART hardware */
    board_uart_init();
}

void putc(char c)
{
    /* Wait until TX buffer is not full */
    while (UART->status & UART_STATUS_TX_FULL)
        ;

    /* Send character */
    UART->data = c;

    /* Handle newline */
    if (c == '\n')
        putc('\r');
}

void puts(const char *str)
{
    while (*str)
        putc(*str++);
}

/* Simple printf implementation supporting %c, %s, %d, %x */
void printf(const char *fmt, ...)
{
    char c;
    int d;
    char *s;
    char buffer[32];
    int i = 0;
    char *p;
    
    __builtin_va_list args;
    __builtin_va_start(args, fmt);

    while ((c = *fmt++) != 0) {
        if (c != '%') {
            putc(c);
            continue;
        }

        c = *fmt++;
        switch (c) {
        case 'c':
            putc(__builtin_va_arg(args, int));
            break;

        case 's':
            p = __builtin_va_arg(args, char *);
            if (p)
                puts(p);
            else
                puts("(null)");
            break;

        case 'd':
            d = __builtin_va_arg(args, int);
            if (d < 0) {
                putc('-');
                d = -d;
            }
            i = 0;
            do {
                buffer[i++] = '0' + (d % 10);
                d /= 10;
            } while (d > 0);
            
            while (i > 0)
                putc(buffer[--i]);
            break;

        case 'x':
            d = __builtin_va_arg(args, int);
            i = 0;
            do {
                int digit = d & 0xf;
                buffer[i++] = digit < 10 ? '0' + digit : 'a' + digit - 10;
                d >>= 4;
            } while (d > 0);
            
            puts("0x");
            while (i > 0)
                putc(buffer[--i]);
            break;

        default:
            putc('%');
            putc(c);
            break;
        }
    }

    __builtin_va_end(args);
}

int getc(void)
{
    /* Wait until there is data */
    while (UART->status & UART_STATUS_RX_EMPTY)
        ;

    /* Return received character */
    return UART->data;
}

int gets(char *buffer, int size)
{
    int i = 0;
    char c;

    while (1) {
        c = getc();
        
        /* Handle backspace */
        if (c == '\b' || c == 0x7F) {
            if (i > 0) {
                i--;
                puts("\b \b");  /* Erase character on terminal */
            }
            continue;
        }
        
        /* Echo character */
        putc(c);
        
        /* Check for end of line */
        if (c == '\r' || c == '\n') {
            buffer[i] = '\0';
            putc('\n');  /* Echo newline */
            return i;
        }
        
        /* Store character if buffer has space */
        if (i < size - 1)
            buffer[i++] = c;
    }
}

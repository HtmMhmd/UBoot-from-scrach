#include "../include/types.h"
#include "../include/config.h"
#include "../include/memory.h"
#include "../include/console.h"

/* Simple bump allocator - just for demo */
static addr_t mem_start;
static addr_t mem_end;
static addr_t mem_current;

int mem_init(void)
{
    mem_start = RAM_START + 0x10000; /* Reserve some memory for bootloader */
    mem_end = RAM_START + RAM_SIZE;
    mem_current = mem_start;
    
    printf("Memory initialized: %dMB available\n", (mem_end - mem_current) / (1024 * 1024));
    return 0;
}

void *malloc(size_t size)
{
    addr_t addr;
    
    /* Align to 8 bytes */
    size = (size + 7) & ~7;
    
    if (mem_current + size >= mem_end)
        return NULL;
    
    addr = mem_current;
    mem_current += size;
    
    return (void *)addr;
}

void free(void *ptr)
{
    /* Not implemented in this simple allocator */
    (void)ptr;
}

void *memcpy(void *dest, const void *src, size_t len)
{
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    
    while (len--)
        *d++ = *s++;
    
    return dest;
}

void *memset(void *dest, int val, size_t len)
{
    uint8_t *d = (uint8_t *)dest;
    
    while (len--)
        *d++ = (uint8_t)val;
    
    return dest;
}

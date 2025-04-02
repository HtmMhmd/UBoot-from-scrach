#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "types.h"

/* Memory initialization */
int mem_init(void);

/* Memory allocation */
void *malloc(size_t size);

/* Memory free */
void free(void *ptr);

/* Memory copy */
void *memcpy(void *dest, const void *src, size_t len);

/* Memory set */
void *memset(void *dest, int val, size_t len);

#endif /* _MEMORY_H_ */

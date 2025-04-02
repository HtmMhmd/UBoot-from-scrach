#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include "types.h"

/* Command function type */
typedef int (*cmd_func_t)(int argc, char *argv[]);

/* Command structure */
typedef struct {
    const char *name;
    const char *help;
    cmd_func_t func;
} cmd_t;

/* Initialize command system */
void cmd_init(void);

/* Process command */
int cmd_process(char *cmdline);

/* Register command */
int cmd_register(const cmd_t *cmd);

/* Lookup command */
cmd_t *cmd_lookup(const char *name);

#endif /* _COMMANDS_H_ */

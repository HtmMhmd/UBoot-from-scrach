#include "../include/types.h"
#include "../include/config.h"
#include "../include/commands.h"
#include "../include/console.h"
#include "../include/memory.h"
#include "../include/board.h"

/* Maximum number of registered commands */
#define MAX_COMMANDS 32

/* Command table */
static cmd_t *cmd_table[MAX_COMMANDS];
static int cmd_count = 0;

/* Built-in command functions */
static int cmd_help(int argc, char *argv[]);
static int cmd_version(int argc, char *argv[]);
static int cmd_reset(int argc, char *argv[]);
static int cmd_boot(int argc, char *argv[]);

/* Built-in commands */
static const cmd_t builtin_cmds[] = {
    {"help", "Show available commands", cmd_help},
    {"version", "Show bootloader version", cmd_version},
    {"reset", "Reset the board", cmd_reset},
    {"boot", "Boot the operating system", cmd_boot},
    {NULL, NULL, NULL}  /* Terminator */
};

void cmd_init(void)
{
    const cmd_t *cmd = builtin_cmds;
    
    /* Register built-in commands */
    while (cmd->name != NULL) {
        cmd_register(cmd);
        cmd++;
    }
    
    printf("Command system initialized with %d commands\n", cmd_count);
}

int cmd_register(const cmd_t *cmd)
{
    if (cmd_count >= MAX_COMMANDS) {
        printf("Error: Command table full\n");
        return -1;
    }
    
    cmd_table[cmd_count++] = (cmd_t *)cmd;
    return 0;
}

cmd_t *cmd_lookup(const char *name)
{
    for (int i = 0; i < cmd_count; i++) {
        /* Compare command names */
        const char *cmd_name = cmd_table[i]->name;
        const char *input = name;
        
        while (*cmd_name && *input && *cmd_name == *input) {
            cmd_name++;
            input++;
        }
        
        if (*cmd_name == '\0' && *input == '\0') {
            return cmd_table[i];
        }
    }
    
    return NULL;
}

int cmd_process(char *cmdline)
{
    char *argv[MAX_ARGS];
    int argc = 0;
    char *p = cmdline;
    cmd_t *cmd;
    
    /* Skip leading spaces */
    while (*p && *p == ' ')
        p++;
    
    /* Empty line */
    if (*p == '\0')
        return 0;
    
    /* Parse arguments */
    argv[argc++] = p;
    while (*p) {
        if (*p == ' ') {
            *p++ = '\0';
            /* Skip spaces */
            while (*p && *p == ' ')
                p++;
            
            if (*p)
                argv[argc++] = p;
            
            if (argc >= MAX_ARGS)
                break;
        } else {
            p++;
        }
    }
    
    /* Look up command */
    cmd = cmd_lookup(argv[0]);
    if (cmd == NULL) {
        printf("Unknown command '%s'. Type 'help' for a list of commands.\n", argv[0]);
        return -1;
    }
    
    /* Execute command */
    return cmd->func(argc, argv);
}

static int cmd_help(int argc, char *argv[])
{
    printf("Available commands:\n");
    printf("------------------\n");
    
    for (int i = 0; i < cmd_count; i++) {
        printf("%-10s - %s\n", cmd_table[i]->name, cmd_table[i]->help);
    }
    
    return 0;
}

static int cmd_version(int argc, char *argv[])
{
    printf("Simple U-Boot version 1.0\n");
    printf("Board: %s\n", BOARD_NAME);
    return 0;
}

static int cmd_reset(int argc, char *argv[])
{
    printf("Resetting system...\n");
    board_reset();
    /* Should not reach here */
    return 0;
}

static int cmd_boot(int argc, char *argv[])
{
    printf("Booting operating system...\n");
    printf("Not implemented in this simple version\n");
    return 0;
}

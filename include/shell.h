#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "config.h"
#include "taskmaster.h"

int shell_run(void);
int shell_parse(char*);
int shell_quit(char *arg);
int shell_status(char *arg);
int shell_reload(char *arg);
int shell_restart(char *arg);
int shell_start(char *arg);
int shell_stop(char *arg);
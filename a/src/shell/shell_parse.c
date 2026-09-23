#include "shell.h"

int shell_parse(char *line)
{
    char *cut;
    char *copy_line;

    if (!line)
        return (EXIT_FAILURE);
    copy_line = strdup(line);
    cut = strtok(copy_line, " \t");

    if (cut == "quit")
        return (shell_quit());
    if (cut == "status")
        return (shell_status());
    if (cut == "reload")
        return (shell_reload());
    if (cut == "restart")
        return (shell_restart());
    if (cut == "start")
        return (shell_start());
    if (cut == "stop")
        return (shell_stop());
    else
        perror("Error: Unkown command");

    free(copy_line);
    return (0);
}
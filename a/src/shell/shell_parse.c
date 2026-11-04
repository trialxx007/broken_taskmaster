#include "shell.h"

int shell_parse(char *line)
{
    char *cut;
    char *arg;
    char *copy_line;

    if (!line)
        return (EXIT_FAILURE);
    copy_line = strdup(line);
    if (!copy_line)
        return (EXIT_FAILURE);
    cut = strtok(copy_line, " \t\n");
    arg = strtok(NULL, " \t\n");
    if (!cut)
    {
        free(copy_line);
        return (0);
    }
    if (strcmp(cut, "quit") == 0)
    {
        free(copy_line);
        return (shell_quit(arg));
    }
    if (strcmp(cut, "status") == 0)
    {
        free(copy_line);
        shell_status(arg);
        return (1);
    }
    if (strcmp(cut, "reload") == 0)
    {
        free(copy_line);
        shell_reload(arg);
        return (1);
    }
    if (strcmp(cut, "restart") == 0)
    {
        free(copy_line);
        shell_restart(arg);
        return (1);
    }
    if (strcmp(cut, "start") == 0)
    {
        free(copy_line);
        shell_start(arg);
        return (1);
    }
    if (strcmp(cut, "stop") == 0)
    {
        free(copy_line);
        shell_stop(arg);
        return (1);
    }
    fprintf(stderr, "Error: Unknown command\n");
    free(copy_line);
    return (0);
}
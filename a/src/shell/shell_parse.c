#include "shell.h"

int shell_parse(char *line)
{
    // char *cut;
    char *copy_line;

    if (!line)
        return (EXIT_FAILURE);
    copy_line = strdup(line);
    // cut = strtok(copy_line, " \t");


    free(copy_line);
    return (0);
}
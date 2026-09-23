#include "shell.h"

int shell_run(void)
{
    char *line_reading;
    char *prompt;

    prompt = "taskmaster>";
    while (1)
    {
        line_reading = readline(prompt);
        if (!line_reading)
            break;
        if (*line_reading)
            add_history(line_reading);
        if (shell_parse(line_reading) == 1)
            break;
        free(line_reading);
    }
    return (0);
}
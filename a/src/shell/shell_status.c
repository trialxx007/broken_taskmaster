#include "shell.h"

int shell_status(void)
{
    for (int i = 0; i < config->programs_count; i++)
        printf("%s: <state>\n", program->name);
    return (0);
}
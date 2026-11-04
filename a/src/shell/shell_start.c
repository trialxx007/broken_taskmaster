#include "shell.h"

int shell_start(char *arg)
{
    if (!g_config)
    {
        perror("Error");
        return (-1);
    }
    for (int i = 0; i < g_config->programs_count; ++i)
    {
        if (strcmp(g_config->programs[i].name, arg) == 0)
        {
            process_start(&g_config->programs[i], i);
            return (0);
        }
        else
            printf("Program not found\n");
    }
    return (0);
}

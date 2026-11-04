#include "shell.h"

int shell_status(char *arg)
{
    if (!g_config)
    {
        fprintf(stderr, "No configuration loaded.\n");
        return (0);
    }
    for (int i = 0; i < g_config->programs_count; i++)
    {
        for (int j = 0; j < g_config->programs[i].numprocs; ++j)
        {
            printf("%s: ", g_config->programs[i].name);
            if (g_config->programs[j].process && g_config->programs[i].process[j].state == STATE_RUNNING)
                printf("RUNNING\n");
            else
                printf("STOPPED\n");
        }
            
    }
    return (0);
}

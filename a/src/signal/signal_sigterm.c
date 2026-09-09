#include "t_signal.h"
#include "config.h"

void handler_sigterm(int sig)
{
    (void)sig;
    for (int i = 0; i < g_config->programs_count; i++)
    {
        for(int j = 0; j < g_config->programs[i].numprocs; j++)
        {
            process_kill(g_config->programs[i].process[j].pid);
        }
    }
    free_config(g_config);
    exit(0);
}


# include "taskmaster.h"

void handler_sigchld(int sig)
{
    (void)sig;
    int status;
    pid_t wait;
    while((wait = waitpid(-1, &status, WNOHANG)))
    {
        bool found = false;
        if (wait <= 0)
            break ;
        for(int i = 0; i < g_config->programs_count; i++)
        {
            for (int j = 0; j < g_config->programs[i].numprocs;j++)
            {
                if (g_config->programs[i].process[j].pid == wait)
                {
                    g_config->programs[i].process[j].state = STATE_STOPPED;
                    found = true;
                    break;
                }
            }
            if (found)
                break;
        }
    }
}
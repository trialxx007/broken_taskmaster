#include "config.h"

int process_stop(pid_t pid, const t_program_config *program)
{
    if (kill(pid, 0) == -1)
        return (EXIT_FAILURE);
    
    if (kill(pid, program->stopsignal) == -1)
        return (EXIT_FAILURE);

    int waited = 0;
    while (waited < program->stoptime)
    {
        int status;
        pid_t result = waitpid(pid, &status, WNOHANG);
        if (result > 0)
            return (EXIT_SUCCESS);
        sleep(1);
        waited++;
    }

    kill(pid, SIGKILL);
    waitpid(pid,NULL,0);
    return (EXIT_SUCCESS);
}



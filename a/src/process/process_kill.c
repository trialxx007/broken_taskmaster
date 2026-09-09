#include "config.h"

int process_kill(pid_t pid)
{
    if (kill(pid, 0) == 0)
    {
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0);
    }
    return (EXIT_SUCCESS);
}
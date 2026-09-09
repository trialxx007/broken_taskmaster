#include "t_signal.h"

void handler_sigchld(int sig);
void handler_sigterm(int sig);

void signal_setup(void)
{
    signal(SIGCHLD, handler_sigchld);
    signal(SIGTERM, handler_sigterm);
}
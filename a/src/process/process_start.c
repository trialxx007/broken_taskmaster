#include "config.h"

int process_start(t_program_config *program, int index)
{
    if (program->process[index].state != STATE_RUNNING && program->process[index].state != STATE_STARTED)
    {
        int process_pid = launch_process(program);
        program->process[index].pid = process_pid;
        program->process[index].state = STATE_RUNNING;
        program->process[index].program_config = program;
        program->process[index].last_launch_time = time(NULL);
    }
    return (EXIT_SUCCESS);
}
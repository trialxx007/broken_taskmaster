#include "config.h"

int process_restart(t_program_config *program, int index)
{
    process_stop(program->process[index].pid, program);
    process_start(program, index);

    return (EXIT_SUCCESS);
}
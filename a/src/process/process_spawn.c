#include "config.h"

pid_t launch_process(t_program_config *program)
{
    pid_t main_pid = fork();

    if (main_pid == -1)
        return (EXIT_FAILURE);

    if (main_pid == 0)
    {
        umask(program->umask);
        chdir(program->workingdir);
        int fd_out = open(program->std_out, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd_out == -1)
            exit(EXIT_FAILURE);
        dup2(fd_out, STDOUT_FILENO);
        int fd_err = open(program->std_err, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd_err == -1)
            exit(EXIT_FAILURE);
        dup2(fd_err, STDERR_FILENO);
        close(fd_out);
        close(fd_err);
        char *argv[] = {program->cmd, NULL};
        execve(program->cmd, argv, program->env);
        exit(EXIT_FAILURE);
    }
    return (main_pid);
}

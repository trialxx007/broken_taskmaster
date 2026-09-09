#include "../../../include/taskmaster.h"
#include "../../../include/utils.h"
#include "../../../include/config.h"
#include <stddef.h>

void	init_program_config(t_program_config *program_config)
{
	if (program_config == NULL)
		return ;
	program_config->name = NULL;
	program_config->cmd = NULL;
	program_config->numprocs = ZERO;
	program_config->umask = ZERO;
	program_config->workingdir = NULL;
	program_config->autostart = false;
	program_config->has_autostart = false;
	program_config->autorestart = AUTORESTART_NEVER;
	program_config->has_autorestart = false;
	program_config->exitcodes = NULL;
	program_config->exitcodes_count = ZERO;
	program_config->exitcodes_capacity = ZERO;
	program_config->startretries = ZERO;
	program_config->starttime = ZERO;
	program_config->stopsignal = ZERO;
	program_config->has_stopsignal = false;
	program_config->stoptime = ZERO;
	program_config->std_out = NULL;
	program_config->std_err = NULL;
	program_config->env = NULL;
	program_config->env_count = ZERO;
	program_config->env_capacity = ZERO;
}

void	init_process_runtime(t_process_runtime *process)
{
	if (process == NULL)
		return ;
	process->pid = ZERO;
	process->state = NULL;
	process->crash_count = ZERO;
	process->last_launch_time = ZERO;
}

void	init_config(t_config *config)
{
	if (config == NULL)
		return ;
	config->programs_count = ZERO;
	config->programs_capacity = ZERO;
	config->total_processes_count = ZERO;
	config->all_processes = NULL;
	config->programs = NULL;
}

void	init_parse_event_struct(t_parse_context* context)
{
	context->done = false;
	context->is_key = false;
	context->is_no_problem = true;
	context->depth = ZERO;
	context->current_key = NULL;
	context->in_sequence = false;
	context->env_key = NULL;
}


bool	config_add_exitcode(t_program_config *program_config, int code)
{
	int*	tmp;

	if (program_config == NULL)
		return (false);
	if (program_config->exitcodes_count == program_config->exitcodes_capacity)
	{
		int	new_capacity;

		if (program_config->exitcodes_capacity == ZERO)
			new_capacity = START_CAPACITY;
		else
			new_capacity = (program_config->exitcodes_capacity * CAPACITY_MULTIPLICATOR);
		tmp = realloc(program_config->exitcodes, new_capacity * sizeof(int));
		if (tmp == NULL)
		{
			write_error_fmt("error: failed to allocate memory.\n");
			return (false);
		}
		program_config->exitcodes = tmp;
		program_config->exitcodes_capacity = new_capacity;
	}
	program_config->exitcodes[program_config->exitcodes_count] = code;
	program_config->exitcodes_count++;
	return (true);
}

bool	parse_block_env(t_program_config* program, const char* key, const char* value)
{
	char	buffer[MAX_SIZE];
	int		res;

	res = snprintf(buffer, sizeof(buffer), "%s=%s", key, value);
	if (res < ZERO || res >= MAX_SIZE)
	{
		write_error_fmt("error: buffer exceed maximum size.\n");
		return (false);
	}
	return (config_add_env_var(program, buffer));
}

bool	config_add_env_var(t_program_config* program_config, const char* var)
{
	char**	tmp;
	char*	copy;

	if (program_config == NULL || var == NULL) return (false);
	if (program_config->env_count == program_config->env_capacity)
	{
		int	new_capacity;

		new_capacity = (program_config->env_capacity == ZERO) ? START_CAPACITY :
			(program_config->env_capacity * CAPACITY_MULTIPLICATOR);
		tmp = realloc(program_config->env, new_capacity * sizeof(char *));
		if (tmp == NULL)
		{
			write_error_fmt("error: failed to allocate memory.\n");
			return (false);
		}
		program_config->env = tmp;
		program_config->env_capacity = new_capacity;
	}
	copy = strdup(var);
	if (copy == NULL) return (false);
	program_config->env[program_config->env_count] = copy;
	program_config->env_count++;
	return (true);
}

#include "config.h"

void	free_program_config(t_program_config *program_config)
{
	int	i;

	if (program_config == NULL)
		return ;
	free(program_config->name);
	free(program_config->cmd);
	free(program_config->workingdir);
	free(program_config->exitcodes);
	free(program_config->std_out);
	free(program_config->std_err);
    free(program_config->process);
	if (program_config->env != NULL)
	{
		i = ZERO;
		while (i < program_config->env_count)
		{
			free(program_config->env[i]);
			i++;
		}
		free(program_config->env);
	}
	init_program_config(program_config);
}

void	free_config(t_config *config)
{
	int	i;

	if (config == NULL)
		return ;
	i = ZERO;
	while (i < config->programs_count)
	{
		free_program_config(&config->programs[i]);
		i++;
	}
	free(config->programs);
	config->programs = NULL;
	free(config->all_processes);
	config->all_processes = NULL;
	free(config);
}
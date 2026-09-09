#include "config.h"
#include "utils.h"

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
	process->state = STATE_STOPPED;
	process->crash_count = ZERO;
	process->last_launch_time = ZERO;
    process->program_config = NULL;
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
int fill_my_program(yaml_document_t *document, yaml_node_t *node, t_program_config *program)
{
    yaml_node_t *tmp_cmd = find_value_by_key(document, node, "cmd");
    if (tmp_cmd)
        program->cmd = strdup(get_scalar_str(tmp_cmd));

    yaml_node_t *tmp_num = find_value_by_key(document, node, "numprocs");
    if (tmp_num)
        program->numprocs = get_scalar_int(tmp_num);

    yaml_node_t *tmp_umask = find_value_by_key(document, node, "umask");
    if (tmp_umask)
        program->umask = get_scalar_octal(tmp_umask);

    yaml_node_t *tmp_dir = find_value_by_key(document, node, "workingdir");
    if (tmp_dir)
        program->workingdir = strdup(get_scalar_str(tmp_dir));

    yaml_node_t *tmp_autostart = find_value_by_key(document, node, "autostart");
    if (tmp_autostart)
        program->autostart = get_scalar_bool(tmp_autostart);

    yaml_node_t *tmp_autorestart = find_value_by_key(document, node, "autorestart");
    if (tmp_autorestart)
        program->autorestart = get_autorestart(tmp_autorestart);

    yaml_node_t *tmp_startretries = find_value_by_key(document, node, "startretries");
    if (tmp_startretries)
        program->startretries = get_scalar_int(tmp_startretries);

    yaml_node_t *tmp_starttime = find_value_by_key(document, node, "starttime");
    if (tmp_starttime)
        program->starttime = get_scalar_int(tmp_starttime);

    yaml_node_t *tmp_stopsignal = find_value_by_key(document, node, "stopsignal");
    if (tmp_stopsignal)
        program->stopsignal = get_signal(tmp_stopsignal);

    yaml_node_t *tmp_stoptime = find_value_by_key(document, node, "stoptime");
    if (tmp_stoptime)
        program->stoptime = get_scalar_int(tmp_stoptime);

    yaml_node_t *tmp_stdout = find_value_by_key(document, node, "stdout");
    if (tmp_stdout)
        program->std_out = strdup(get_scalar_str(tmp_stdout));

    yaml_node_t *tmp_stderr = find_value_by_key(document, node, "stderr");
    if (tmp_stderr)
        program->std_err = strdup(get_scalar_str(tmp_stderr));

    yaml_node_t *tmp_env = find_value_by_key(document, node, "env");

    if (tmp_env && tmp_env->type == YAML_MAPPING_NODE)
    {
        int i = 0;
        program->env_count = tmp_env->data.mapping.pairs.top - tmp_env->data.mapping.pairs.start;
        program->env = malloc(sizeof(char *) * program->env_count);
        if (!program->env)
            return (EXIT_FAILURE);

        yaml_node_pair_t *pair_env = tmp_env->data.mapping.pairs.start;
        while (pair_env < tmp_env->data.mapping.pairs.top)
        {
            yaml_node_t *key_node = yaml_document_get_node(document, pair_env->key);
            const char *ki = get_scalar_str(key_node);

            yaml_node_t *val_node = yaml_document_get_node(document, pair_env->value);
            const char *val = get_scalar_str(val_node);

            size_t size_of_node = strlen(ki) + strlen(val) + 2;
            program->env[i] = malloc(size_of_node);
            if (!program->env[i])
                return (EXIT_FAILURE);
            snprintf(program->env[i], size_of_node, "%s=%s", ki, val);
            i++;
            pair_env++;
        }
    }

    yaml_node_t *tmp_exitcodes = find_value_by_key(document, node, "exitcodes");
    if (!tmp_exitcodes)
        return (EXIT_FAILURE);
    if (tmp_exitcodes->type == YAML_SCALAR_NODE)
    {
        program->exitcodes_count = 1;
        int *tab = malloc(sizeof(int) * 1);
        tab[0] = get_scalar_int(tmp_exitcodes);
        program->exitcodes = tab;
    }
    else if (tmp_exitcodes->type == YAML_SEQUENCE_NODE)
    {
        yaml_node_item_t *pair_items = tmp_exitcodes->data.sequence.items.start;
        program->exitcodes_count = tmp_exitcodes->data.sequence.items.top - pair_items;
        program->exitcodes = malloc(sizeof(int) * program->exitcodes_count);
        if (!program->exitcodes)
            return (EXIT_FAILURE);
        int i = 0;
        while (pair_items < tmp_exitcodes->data.sequence.items.top)
        {
            yaml_node_t *exit_node = yaml_document_get_node(document, *pair_items);
            program->exitcodes[i] = get_scalar_int(exit_node);
            i++;
            pair_items++;
        }
    }
    return (EXIT_SUCCESS);
}

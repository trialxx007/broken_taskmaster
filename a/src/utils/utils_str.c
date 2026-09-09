# include <stdio.h>
# include "utils.h"
# include "config.h"

void	print_msg(void)
{
	printf("Hello, taskmaster from a!\n");
}

const char *get_scalar_str(yaml_node_t *node)
{
	if (!node || node->type != YAML_SCALAR_NODE)
		return (NULL);
	return ((const char *)node->data.scalar.value);
}

int get_scalar_int(yaml_node_t *node)
{
	const char *str_node = get_scalar_str(node);
	
	if (!str_node)
		return (-1);
	
	int convert = atoi(str_node);

	return (convert);
}

bool get_scalar_bool(yaml_node_t *node)
{
	const char *str_node = get_scalar_str(node);

	if (!str_node)
		return (false);
	
	if (strcmp(str_node, "true") == 0)
		return (true);
	
	return (false);
}

mode_t get_scalar_octal(yaml_node_t *node)
{
	const char *str_node = get_scalar_str(node);
	
	if (!str_node)
		return (0);

	long convert = strtol(str_node, NULL, OCTAL);

	return (convert);
}

t_autorestart get_autorestart(yaml_node_t *node)
{
	const char *str_node = get_scalar_str(node);
	
	if (!str_node)
		return (AUTORESTART_UNKNOWN);
	
	if (strcmp(str_node, "unexpected") == 0)
		return (AUTORESTART_UNEXPECTED);
	
	else if (strcmp(str_node, "always") == 0)
		return (AUTORESTART_ALWAYS);
	
	else if (strcmp(str_node, "never") == 0)
		return (AUTORESTART_NEVER);
	return (AUTORESTART_UNKNOWN);
}

int get_signal(yaml_node_t *node)
{
	const char *str_node = get_scalar_str(node);
	
	if (!str_node)
		return (-1);
	
	if (strcmp(str_node, "HUP") == 0)
		return (SIGHUP);
	
	else if(strcmp(str_node, "INT") == 0)
		return (SIGINT);
	
	else if(strcmp(str_node, "QUIT") == 0)
		return (SIGQUIT);
	
	else if(strcmp(str_node, "KILL") == 0)
		return (SIGKILL);
	
	else if(strcmp(str_node, "USR1") == 0)
		return (SIGUSR1);
	
	else if(strcmp(str_node, "USR2") == 0)
		return (SIGUSR2);
	
	else if(strcmp(str_node, "TERM") == 0)
		return (SIGTERM);

	return (-1);
}

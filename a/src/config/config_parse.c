#include "config.h"

t_config	*config_create(void)
{
	t_config	*config;

	config = malloc(sizeof(t_config));
	if (config == NULL)
	{
		write_error_fmt("error: failed to allocate memory.\n");
		return (NULL);
	}
	init_config(config);
	return (config);
}

yaml_node_t *find_value_by_key(yaml_document_t *document, yaml_node_t *mapping, const char *key)
{
    yaml_node_pair_t *pair = mapping->data.mapping.pairs.start;

    while (pair < mapping->data.mapping.pairs.top)
    {
        yaml_node_t *key_node  = yaml_document_get_node(document, pair->key);
        if (strcmp((const char *)key_node->data.scalar.value, key) == 0)
            return (yaml_document_get_node(document, pair->value));
        pair++;
    }
    return (NULL);
}

int parse_programs(yaml_document_t *document, yaml_node_t *node, t_config *config)
{
    yaml_node_t *programs_node = find_value_by_key(document, node, "programs");

    if (programs_node != NULL && programs_node->type == YAML_MAPPING_NODE)
    {
        config->programs_count = programs_node->data.mapping.pairs.top - programs_node->data.mapping.pairs.start;

        config->programs = malloc(sizeof(t_program_config) * config->programs_count);
        if (!config->programs)
            return (-1);

        yaml_node_pair_t *pair = programs_node->data.mapping.pairs.start;
        int i = 0;
        while (pair < programs_node->data.mapping.pairs.top)
        {
            yaml_node_t *pair_node = yaml_document_get_node(document, pair->key);
            memset(&config->programs[i], 0, sizeof(t_program_config));
            config->programs[i].name = strdup((const char *)pair_node->data.scalar.value);

            yaml_node_t *value_node = yaml_document_get_node(document, pair->value);
            fill_my_program(document, value_node, &config->programs[i]);
            pair++;
            i++;
        }
        return (EXIT_SUCCESS);
    }
    return (EXIT_FAILURE);
}

int process_file(char *file)
{
    int ret = EXIT_SUCCESS;
    if (access(file, F_OK) != 0)
    {
        fprintf(stderr, "Error: File doesn't exist\n");
        exit(EXIT_FAILURE);
    }

    FILE *f = fopen(file, "r");

    if (f == NULL)
    {
        fprintf(stderr, "Error: couldn't open the file\n");
        return (EXIT_FAILURE);
    }

    fseek(f, 0,SEEK_END);
    long int file_size = ftell(f);
    if (file_size == 0)
    {
        printf("Error: Empty file\n");
        ret = EXIT_FAILURE;
        goto cleanup_file;
    }
    if (file_size < 0)
    {
        printf("Error: ftell() failed\n");
        ret = EXIT_FAILURE;
        goto cleanup_file;
    }
    rewind(f);

    char *yaml_content = (char *)malloc(file_size + 1);
    if (!yaml_content)
    {
        ret = EXIT_FAILURE;
        goto cleanup_file;
    }

    size_t reading = fread(yaml_content, sizeof(char), file_size, f);
    yaml_content[reading] = '\0';


    yaml_parser_t parser;
    yaml_document_t document;

    if (!yaml_parser_initialize(&parser))
    {
        fputs("Error: Yaml parse failed\n", stderr);
        ret = EXIT_FAILURE;
        goto cleanup_parser;
    }

    yaml_parser_set_input_string(&parser, (const unsigned char *)yaml_content, reading);

    if (!yaml_parser_load(&parser, &document))
    {
        fputs("Error: Load failed\n", stderr);
        ret = EXIT_FAILURE;
        goto cleanup_document;
    }

    yaml_node_t *root = yaml_document_get_root_node(&document);
    if (root == NULL)
    {
        ret = EXIT_FAILURE;
        goto cleanup_document;
    }
    t_config *config = config_create();

    if (parse_programs(&document, root, config) != EXIT_SUCCESS)
    {
        ret = EXIT_FAILURE;
        goto cleanup_document;
    }
    g_config = config;
    signal(SIGCHLD, handler_sigchld);
    for (int i = 0; i < config->programs_count; i++)
    {
        printf("Program: %s\n", config->programs[i].name);
        printf("  cmd: %s\n", config->programs[i].cmd);
        printf("  numprocs: %d\n", config->programs[i].numprocs);
        printf("  autostart: %d\n", config->programs[i].autostart);
        printf("  stopsignal: %d\n", config->programs[i].stopsignal);
        printf("  exitcodes_count: %d\n", config->programs[i].exitcodes_count);
        for (int j = 0; j < config->programs[i].exitcodes_count; j++)
            printf("  exitcode[%d]: %d\n", j, config->programs[i].exitcodes[j]);
        for (int j = 0; j < config->programs[i].env_count; j++)
            printf("  env[%d]: %s\n", j, config->programs[i].env[j]);
    }

    for (int i = 0; i < config->programs_count ; i++)
    {
        if (config->programs[i].autostart)
        {
            config->programs[i].process = malloc(sizeof(t_process_runtime) * config->programs[i].numprocs);
            memset(config->programs[i].process, 0 ,sizeof(t_process_runtime) * config->programs[i].numprocs);
            for (int j = 0; j < config->programs[i].numprocs; j++)
            {
                config->programs[i].process[j].pid = launch_process(&config.programs[i]);
                config->programs[i].process[j].program_config = &config.programs[i];
                printf("Launched %s[%d] with pid %d\n", config.programs[i].name, j, config.programs[i].process[j].pid);
            }
        }
    }

    cleanup_document:
        yaml_document_delete(&document);
    cleanup_parser:
        yaml_parser_delete(&parser);
    cleanup_file:
        fclose(f);

    return (ret);
}

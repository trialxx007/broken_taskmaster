#ifndef CONFIG_H
# define CONFIG_H

# include "taskmaster.h"
# include <yaml.h>
# include <unistd.h>
# include <time.h>

/*
 * ============================================================
 * CONTRACT INTERFACE — DO NOT MODIFY
 * ============================================================
 * These prototypes form the public API of the config module.
 * They are used across the entire codebase and must remain
 * stable. Any change to a signature breaks all callers.
 *
 * Rules:
 *   - Never rename, remove, or change the signature of any
 *     function declared in this section.
 *   - New public functions may be added below the contract
 *     section only if they follow the same naming convention
 *     and are documented with their purpose and return value.
 * ============================================================
 */

void		init_program_config(t_program_config *program_config);
void		free_program_config(t_program_config *program_config);

void		init_config(t_config *config);
void		free_config(t_config *config);

void        init_process_runtime(t_process_runtime *process);
void		init_parse_event_struct(t_parse_context* context);
t_config	*config_parse(const char *filename);

/*
 * ============================================================
 * INTERNAL HELPERS — MODIFY WITH CARE
 * ============================================================
 * These functions are internal to the config module.
 * You may add, rename, or refactor them freely, but ensure
 * that the contract functions above continue to work correctly.
 * ============================================================
 */
void		handle_struct_events(t_parse_context* context);
bool	parse_scalar_event(t_config* conf, t_parse_context* context, const char* val);
bool	config_parse_events(t_config* config, t_parse_context* context);
bool	config_parse_program(t_config *config, const char *name);
bool	config_set_field(t_program_config *program, const char *key, const char *value);
int	signal_from_string(const char *value);
t_config	*config_create(void);
bool	config_open_file(const char *filename, yaml_parser_t *parser, FILE **file);
void	config_close_file(yaml_parser_t *parser, FILE **file);
bool	config_add_exitcode(t_program_config *program_config, int code);
bool	parse_block_env(t_program_config* program, const char* key, const char* value);
bool	config_add_env_var(t_program_config* program_config, const char* var);
yaml_node_t	*find_value_by_key(yaml_document_t *document, yaml_node_t *mapping, const char *key);
int			fill_my_program(yaml_document_t *document, yaml_node_t *node, t_program_config *program);
int			parse_programs(yaml_document_t *document, yaml_node_t *node, t_config *config);
int			process_file(char *file);
pid_t   	launch_process(t_program_config *program);

int		process_stop(pid_t pid, const t_program_config *program);
int		process_start(t_program_config *program, int index);
int		process_kill(pid_t pid);
int		process_restart(t_program_config *program, int index);
#endif

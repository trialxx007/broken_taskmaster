#ifndef TASKMASTER_H
# define TASKMASTER_H

# include <sys/stat.h>
# include <sys/types.h>
# include <stdbool.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <yaml.h>
# include <signal.h>

# define MAX_SIZE				4096
# define MAX_UCHAR				255
# define MAX_UMASK				511

# define TSK_OK					0
# define TSK_ERROR				-1

# define EXPECTED_ARG_COUNT		2

# define START_CAPACITY			4
# define CAPACITY_MULTIPLICATOR	2

# define ZERO					0
# define OCTAL					8
# define DECIMAL				10

# define FIRST_DEPTH			1
# define SECOND_DEPTH			2
# define THIRD_DEPTH			3
# define MAX_DEPTH				4

# define SIGHUP					1
# define SIGINT					2
# define SIGQUIT				3
# define SIGKILL				9
# define SIGUSR1				10
# define SIGUSR2				12
# define SIGTERM				15
# define SIGCHLD				17

typedef struct s_program_config t_program_config;

typedef enum	e_autorestart
{
	AUTORESTART_ALWAYS,
	AUTORESTART_NEVER,
	AUTORESTART_UNEXPECTED,
	AUTORESTART_UNKNOWN,
}				t_autorestart;

typedef enum	e_process_state
{
	STATE_STOPPED,
	STATE_STARTED,
	STATE_RUNNING,
	STATE_FATAL,
}				t_process_state;

typedef struct	s_process_runtime
{
	pid_t					pid;
	t_process_state			state;
	int						crash_count;
	time_t					last_launch_time;
	const t_program_config	*program_config;
}				t_process_runtime;

typedef struct	s_program_config
{
	char			*name;
	char			*cmd;
	int				numprocs;
	mode_t			umask;
	char			*workingdir;
	bool			autostart;
	bool			has_autostart;
	t_autorestart	autorestart;
	bool			has_autorestart;
	int				*exitcodes;
	int				exitcodes_count;
	int				exitcodes_capacity;
	int				startretries;
	int				starttime;
	int				stopsignal;
	bool			has_stopsignal;
	int				stoptime;
	char			*std_out;
	char			*std_err;
	char			**env;
	int				env_count;
	int				env_capacity;
	t_process_runtime *process;
}				t_program_config;

typedef struct	s_config
{
	int					programs_count;
	int					programs_capacity;
	int					total_processes_count;
	t_process_runtime*	all_processes;
	t_program_config*	programs;
}				t_config;

typedef struct	s_parse_context
{
	yaml_event_t	event;
	yaml_parser_t*	parser;
	bool			done;
	bool			is_key;
	bool			is_no_problem;
	int				depth;
	char			*current_key;
	bool			in_sequence;
	char			*env_key;
}				t_parse_context;

extern t_config *g_config;
extern const char *g_config_filename;
void handler_sigchld(int sig);
#endif

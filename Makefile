NAME			:= taskmaster

CC				:= gcc

CFLAGS			:= -Wall -Wextra -Werror -g

INCLUDES		:= -I./include

INCLUDES		+= -I$(HOME)/.local/include

CFLAGS 			+= $(INCLUDES)

YAMLFLAGS		:= -L$(HOME)/.local/lib -lyaml

CFLAGS			+= $(YAMLFLAGS)

SRCS_A			:= a/src/main.c \
					\
					a/src/config/config_parse.c \
					a/src/config/config_validate.c \
					a/src/config/config_reload.c \
					a/src/config/config_diff.c \
					a/src/config/config_free.c \
					a/src/config/config_init.c \
					\
					a/src/process/process_spawn.c \
					a/src/process/process_start.c \
					a/src/process/process_stop.c \
					a/src/process/process_kill.c \
					a/src/process/process_restart.c \
					a/src/process/process_env.c \
					\
					a/src/monitor/monitor_init.c \
					a/src/monitor/monitor_check.c \
					a/src/monitor/monitor_death.c \
					a/src/monitor/monitor_retry.c \
					a/src/monitor/monitor_status.c \
					\
					a/src/logger/logger_init.c \
					a/src/logger/logger_info.c \
					a/src/logger/logger_warn.c \
					a/src/logger/logger_error.c \
					a/src/logger/logger_close.c \
					\
					a/src/signal/signal_setup.c \
					a/src/signal/signal_sighup.c \
					a/src/signal/signal_sigchld.c \
					a/src/signal/signal_sigterm.c \
					\
					a/src/shell/shell_run.c \
					a/src/shell/shell_parse.c \
					a/src/shell/shell_status.c \
					a/src/shell/shell_start.c \
					a/src/shell/shell_stop.c \
					a/src/shell/shell_restart.c \
					a/src/shell/shell_reload.c \
					a/src/shell/shell_quit.c \
					\
					a/src/utils/utils_str.c \
					a/src/utils/utils_signal.c \
					a/src/utils/utils_umask.c \
					a/src/utils/utils_error.c \
					a/src/utils/utils_file_check.c \
					a/src/utils/utils_file_validate.c \
					a/src/utils/globals.c

SRCS_B			:= b/src/main.c \
					\
					b/src/config/config_parse.c \
					b/src/config/config_validate.c \
					b/src/config/config_reload.c \
					b/src/config/config_diff.c \
					b/src/config/config_free.c \
					b/src/config/config_init.c \
					\
					b/src/process/process_spawn.c \
					b/src/process/process_start.c \
					b/src/process/process_stop.c \
					b/src/process/process_kill.c \
					b/src/process/process_restart.c \
					b/src/process/process_env.c \
					\
					b/src/monitor/monitor_init.c \
					b/src/monitor/monitor_check.c \
					b/src/monitor/monitor_death.c \
					b/src/monitor/monitor_retry.c \
					b/src/monitor/monitor_status.c \
					\
					b/src/logger/logger_init.c \
					b/src/logger/logger_info.c \
					b/src/logger/logger_warn.c \
					b/src/logger/logger_error.c \
					b/src/logger/logger_close.c \
					\
					b/src/signal/signal_setup.c \
					b/src/signal/signal_sighup.c \
					b/src/signal/signal_sigchld.c \
					b/src/signal/signal_sigterm.c \
					\
					b/src/shell/shell_run.c \
					b/src/shell/shell_parse.c \
					b/src/shell/shell_status.c \
					b/src/shell/shell_start.c \
					b/src/shell/shell_stop.c \
					b/src/shell/shell_restart.c \
					b/src/shell/shell_reload.c \
					b/src/shell/shell_quit.c \
					\
					b/src/utils/utils_str.c \
					b/src/utils/utils_signal.c \
					b/src/utils/utils_umask.c \
					b/src/utils/utils_error.c \
					b/src/utils/utils_file_check.c \
					b/src/utils/utils_file_validate.c

SRCS_FINAL		:= final/src/main.c \
					\
					final/src/config/config_parse.c \
					final/src/config/config_validate.c \
					final/src/config/config_reload.c \
					final/src/config/config_diff.c \
					final/src/config/config_free.c \
					final/src/config/config_init.c \
					\
					final/src/process/process_spawn.c \
					final/src/process/process_start.c \
					final/src/process/process_stop.c \
					final/src/process/process_kill.c \
					final/src/process/process_restart.c \
					final/src/process/process_env.c \
					\
					final/src/monitor/monitor_init.c \
					final/src/monitor/monitor_check.c \
					final/src/monitor/monitor_death.c \
					final/src/monitor/monitor_retry.c \
					final/src/monitor/monitor_status.c \
					\
					final/src/logger/logger_init.c \
					final/src/logger/logger_info.c \
					final/src/logger/logger_warn.c \
					final/src/logger/logger_error.c \
					final/src/logger/logger_close.c \
					\
					final/src/signal/signal_setup.c \
					final/src/signal/signal_sighup.c \
					final/src/signal/signal_sigchld.c \
					final/src/signal/signal_sigterm.c \
					\
					final/src/shell/shell_run.c \
					final/src/shell/shell_parse.c \
					final/src/shell/shell_status.c \
					final/src/shell/shell_start.c \
					final/src/shell/shell_stop.c \
					final/src/shell/shell_restart.c \
					final/src/shell/shell_reload.c \
					final/src/shell/shell_quit.c \
					\
					final/src/utils/utils_str.c \
					final/src/utils/utils_signal.c \
					final/src/utils/utils_umask.c \
					final/src/utils/utils_error.c \
					final/src/utils/utils_file_check.c \
					final/src/utils/utils_file_validate.c

SRCS_DIR_A		:= ./a/src

SRCS_DIR_B 		:= ./b/src

SRCS_DIR_FINAL	:= ./final/src

OBJS_DIR 		:= ./obj

OBJS_A			:= $(patsubst a/src/%.c, $(OBJS_DIR)/a/%.o, $(SRCS_A))

OBJS_B 			:= $(patsubst b/src/%.c, $(OBJS_DIR)/b/%.o, $(SRCS_B))

OBJS_FINAL		:= $(patsubst final/src/%.c, $(OBJS_DIR)/final/%.o, $(SRCS_FINAL))

RM				:= rm -rf

all : a b final

a : taskmaster_a

taskmaster_a : $(OBJS_A)
	$(CC) $(OBJS_A) $(YAMLFLAGS) -o $(NAME)_a

b : taskmaster_b

taskmaster_b : $(OBJS_B)
	$(CC) $(OBJS_B) $(YAMLFLAGS) -o $(NAME)_b

final : taskmaster

taskmaster : $(OBJS_FINAL)
	$(CC) $(OBJS_FINAL) $(YAMLFLAGS) -o $(NAME)

$(OBJS_DIR)/a/%.o: a/src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/b/%.o: b/src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/final/%.o: final/src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	$(RM) $(OBJS_DIR)

fclean : clean
	$(RM) $(NAME)_a $(NAME)_b $(NAME)

re : fclean all

.PHONY: all clean fclean re

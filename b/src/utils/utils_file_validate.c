#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include "../../../include/utils.h"
#include "../../../include/taskmaster.h"

int	validate_arg(int arg_count, char **arg_value)
{
	struct stat	statbuf;

	
	memset(&statbuf, 0, sizeof(statbuf));
	if (arg_value == NULL)
		return (TSK_ERROR);
	if (has_correct_arg_nbr(arg_count) == TSK_ERROR)
	{
		write_error_fmt("error: usage ./taskmaster <config.yaml>.\n");
		return (TSK_ERROR);
	}
	if (has_yaml_extension(arg_value[1]) == TSK_ERROR)
	{
		write_error_fmt("error: config file must have a .yaml or .yml extension.\n");
		return (TSK_ERROR);
	}
	if (is_file_existing(arg_value[1]) == TSK_ERROR)
	{
		if (errno == ENOENT)
			write_error_fmt("error: configuration file does not exist.\n");
		else if (errno == ENAMETOOLONG)
			write_error_fmt("error: configuration file path is too long.\n");
		else if (errno == ELOOP)
			write_error_fmt("error: too many symbolic links in path.\n");
		else
			write_error_fmt("error: %s: %s\n", arg_value[1], strerror(errno));
		return (TSK_ERROR);
	}
	if (is_file_readable(arg_value[1]) == TSK_ERROR)
	{
		if (errno == EACCES)
			write_error_fmt("error: permission denied on configuration file.\n");
		else
			write_error_fmt("error: %s: %s\n", arg_value[1], strerror(errno));
		return (TSK_ERROR);
	}
	if (is_file_regular(arg_value[1], &statbuf) == TSK_ERROR)
	{
		if (S_ISDIR(statbuf.st_mode))
			write_error_fmt("error: configuration file is a directory.\n");
		else if (S_ISFIFO(statbuf.st_mode))
			write_error_fmt("error: configuration file is a pipe.\n");
		else
			write_error_fmt("error: configuration file is not a regular file.\n");
		return (TSK_ERROR);
	}
	return (TSK_OK);
}

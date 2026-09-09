#include <sys/stat.h>
#include <unistd.h>
#include "../../../include/utils.h"
#include "../../../include/taskmaster.h"

int	has_correct_arg_nbr(int arg_count)
{
	if (arg_count == EXPECTED_ARG_COUNT)
		return (TSK_OK);
	return (TSK_ERROR);
}

int	has_yaml_extension(const char *filename)
{
	const char	*dot;
	size_t		ext_yaml_length;
	size_t		ext_yml_length;
	size_t		dot_length;

	if (!filename || !*filename)
		return (TSK_ERROR);
	ext_yaml_length = sizeof(".yaml") - 1;
	ext_yml_length = sizeof(".yml") - 1;
	dot = strrchr(filename, '.');
	if (!dot || dot[1] == '\0')
		return (TSK_ERROR);
	dot_length = strlen(dot);
	if (dot_length != ext_yaml_length && dot_length != ext_yml_length)
		return (TSK_ERROR);
	if (strncmp(dot, ".yaml", ext_yaml_length) != 0
		&& strncmp(dot, ".yml", ext_yml_length) != 0)
		return (TSK_ERROR);
	return (TSK_OK);
}

int	is_file_existing(const char *filename)
{
	if (!filename || !*filename)
		return (TSK_ERROR);
	if (access(filename, F_OK) == -1)
		return (TSK_ERROR);
	return (TSK_OK);
}

int	is_file_readable(const char *filename)
{
	if (!filename || !*filename)
		return (TSK_ERROR);
	if (access(filename, R_OK) == -1)
		return (TSK_ERROR);
	return (TSK_OK);
}

int	is_file_regular(const char *filename, struct stat *statbuf)
{
	if (!filename || !*filename)
		return (TSK_ERROR);
	if (stat(filename, statbuf) == -1)
		return (TSK_ERROR);
	if (!S_ISREG(statbuf->st_mode))
		return (TSK_ERROR);
	return (TSK_OK);
}

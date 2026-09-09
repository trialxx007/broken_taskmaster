#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "../../../include/taskmaster.h"

static int	write_std_fmt(int fd, const char* fmt, va_list arg_pointer)
{
	char	buffer[MAX_SIZE];
	int		length;
	ssize_t	written;

	length = vsnprintf(buffer, sizeof(buffer), fmt, arg_pointer);
	if (length < 0 || length >= MAX_SIZE)
		return (TSK_ERROR);
	written = write(fd, buffer, length);
	if (written == -1 || written != length)
		return (TSK_ERROR);
	return (TSK_OK);
}

int	write_error_fmt(const char* fmt, ...)
{
	int		ret;
	va_list	arg_pointer;

	va_start(arg_pointer, fmt);
	ret = write_std_fmt(STDERR_FILENO, fmt, arg_pointer);
	va_end(arg_pointer);
	if (ret == TSK_ERROR)
		return (TSK_ERROR);
	return (TSK_OK);
}

int	write_success_fmt(const char* fmt, ...)
{
	int		ret;
	va_list	arg_pointer;

	va_start(arg_pointer, fmt);
	ret = write_std_fmt(STDOUT_FILENO, fmt, arg_pointer);
	va_end(arg_pointer);
	if (ret == TSK_ERROR)
		return (TSK_ERROR);
	return (TSK_OK);
}

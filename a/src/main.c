#include <stdio.h>
#include "utils.h"

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		perror("Error: Missing arguments");
		return (EXIT_FAILURE);
	}
	
	if (check_file_extension(av[1], "yaml") == EXIT_FAILURE)
		exit(EXIT_FAILURE);

	if (process_file(av[1]) == EXIT_FAILURE)
		return (EXIT_FAILURE);

	// printf("Well done !\n");

	return (0);
}

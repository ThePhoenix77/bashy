#include "bashy.h"

// Save the current terminal attributes to the global structure
void	save_tty(t_global *global)
{
	if (tcgetattr(STDIN_FILENO, &global->t_termios) == -1) // Get terminal attributes
	{
		perror("bashy: tcgetattr"); // Print error if unsuccessful
		malloc_failed(global); // Handle memory failure
	}
}

// Restore the saved terminal attributes from the global structure
void	reset_tty(t_global *global)
{
	if (tcsetattr(STDIN_FILENO, TCSANOW, &global->t_termios) == -1) // Set terminal attributes
	{ 
		perror("bashy: tcsetattr"); // Print error if unsuccessful
		malloc_failed(global); // Handle memory failure
	}
}

// Handle memory allocation failure
void	malloc_failed(t_global *global)
{
	free_memory(global); // Free allocated memory in the global structure
	if (global->content)
		free(global->content); // Free content if allocated
	if (global->env_list)
		free_env_list(&global->env_list); // Free the environment list if allocated
	global->exit_status = 1; // Set exit status to 1 to indicate failure
	exit(1); // Exit the program
}

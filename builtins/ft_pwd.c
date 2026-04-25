#include "bashy.h"

/**
 * ft_pwd - Prints the current working directory to standard output.
 * @global: Pointer to global state structure, which contains the shell's exit status.
 * 
 * This function retrieves the current working directory using `getcwd` and
 * prints it. If an error occurs (e.g., due to insufficient permissions or
 * other system-related issues), it prints an error message to stderr and 
 * sets the shell's exit status accordingly.
 * 
 * Return: 0 on success, -1 on failure.
 */
int	ft_pwd(t_global *global)
{
	char	*cwd;

	// Retrieve the current working directory. 
	// Passing NULL and 0 allows getcwd to dynamically allocate memory for the result.
	cwd = getcwd(NULL, 0);
	if (cwd == NULL) // Check if getcwd failed (e.g., due to permission issues)
	{
		perror("getcwd"); // Print error message to stderr
		global->exit_status = 1; // Set the exit status to indicate failure
		return (-1); // Return error code
	}
	
	// Print the current working directory
	printf("%s\n", cwd);

	// Free the memory allocated by getcwd
	free(cwd);

	// Set exit status to 0 (success) and return success code
	global->exit_status = 0;
	return (0);
}

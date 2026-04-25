#include "bashy.h"

// Create a child process to execute a command
bool	create_child(t_global *global, char **cmd_ars)
{
	int	child;

	child = fork(); // Create a new process
	if (child < 0)
		return (perror("bashy: fork:"), FAILURE); // Handle fork failure

	sig_ign(); // Ignore signals in the parent
	save_tty(global); // Save the terminal state for the global context

	if (child == 0) // Child process
	{
		sig_dfl(); // Reset signal handling to default for the child
		handle_redir_once(global); // Handle redirection for the command

		// Check if command arguments exist and process them
		if (global->root->cmd_args)
		{
			if (cmd_ars[0] && cmd_ars[0][0] == '\x02') // Check for special case
			{
				global->exit_status = 127; // Set exit status
				ft_cmd_err(cmd_ars[0]); // Handle command error
			}
			execute_cmd(global, cmd_ars, global->root->type); // Execute the command
		}
		exit(0); // Exit child process
	}
	
	waitpid(child, &global->exit_status, 0); // Wait for child to finish
	global->exit_status = exit_status(global->exit_status, global); // Update exit status
	(signal(SIGINT, signals_handler), signal(SIGQUIT, signals_handler)); // Restore signal handling
	return (SUCCESS); // Return success
}

// Execute a single command
int	execute_one_cmd(t_global *global)
{
	char	**cmd_ars;

	cmd_ars = NULL; // Initialize command arguments array

	// Get command arguments from the global context
	if (global->root->cmd_args)
		cmd_ars = get_args_arr(global, global->root->cmd_args, global->root->type);
	
	// Check if the command is a built-in and execute it if so
	if (execute_one_builtin(global, cmd_ars, global->root) == SUCCESS)
		return (free_2d_array(&cmd_ars), SUCCESS); // Free memory and return success

	create_child(global, cmd_ars); // Create a child process to execute the command
	free_2d_array(&cmd_ars); // Free command arguments memory
	return (SUCCESS); // Return success
}

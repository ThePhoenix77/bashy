#include "bashy.h"

// Handle fork failure by printing an error and closing the pipe
void	fork_failed(t_global *global)
{
	perror("bashy: fork"); // Print fork error
	close_pipe(global->pipe);   // Close the pipe
}

// Wait for the two child processes to complete and update exit status
void	wiat_childs(t_global *global, pid_t child1, pid_t child2)
{
	int	status;

	status = 0; // Initialize status variable
	close_pipe(global->pipe); // Close the pipe
	waitpid(child1, &status, 0); // Wait for the first child
	waitpid(child2, &status, 0); // Wait for the second child
	global->exit_status = exit_status(status, global); // Update exit status
	sig_hnd(); // Handle signals
}

// Execute a pipeline of commands
int	execute_pipe(t_global *global, t_tree *root)
{
	pid_t	child1; // Process ID for the first child
	pid_t	child2; // Process ID for the second child

	if (pipe(global->pipe) < 0) // Create a pipe
		return (perror("bashy: pipe:"), -1); // Handle pipe creation error
	child1 = fork(); // Fork the first child
	if (child1 == -1) // Check for fork failure
		return (fork_failed(global), -1);
	sig_ign(); // Ignore signals for the parent process
	if (child1 == 0) // In the first child process
	{
		sig_dfl(); // Restore default signal handling
		(execute_left(global, root, global->pipe), exit(global->exit_status)); // Execute the left command
	}
	child2 = fork(); // Fork the second child
	if (child2 == -1) // Check for fork failure
		return (fork_failed(global), -1);
	if (child2 == 0) // In the second child process
	{
		sig_dfl(); // Restore default signal handling
		(execute_right(global, root, global->pipe), exit(global->exit_status)); // Execute the right command
	}
	else // In the parent process
		wiat_childs(global, child1, child2); // Wait for both children to finish
	return (1); // Return success
}

// Execute commands in the syntax tree
int	execute_tree(t_global *global, t_tree *root)
{
	if (root->type == PIPE_LINE) // Check if the node is a pipeline
		return (execute_pipe(global, root)); // Execute the pipeline
	else
		return (execute_cmd(global, root->cmd_args, root->type)); // Execute a single command
}

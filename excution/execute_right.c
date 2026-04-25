#include "bashy.h"

// Duplicate the output file descriptor for the right node
bool	dup_right_out(t_tree *root)
{
	if (root->right->fds.out != -1) // Check if the output file descriptor is valid
	{
		if (dup2(root->right->fds.out, 1) < 0) // Duplicate to standard output
			return (perror("bashy: dup2:"), FAILURE); // Handle duplication error
		close(root->right->fds.out); // Close the original file descriptor
	}
	return (SUCCESS); // Return success
}

// Execute the command in the right node of the syntax tree
int	execute_right(t_global *global, t_tree *root, int *pipe)
{
	// Check if the right node is a command or built-in
	if (root->right->type == CMD || root->right->type == BUILTIN)
	{
		// Handle redirection if it exists
		if (root->right->redir)
		{
			// Process the redirection and update the file descriptors
			if (handle_redirection(root->right->redir, &root->right->fds) == SUCCESS)
			{
				// Duplicate output file descriptor
				if (dup_right_out(root) == FAILURE)
					return (-1);
				// Check if there's an input file descriptor and duplicate it
				if (root->right->fds.in != -1)
				{
					if (dup2(root->right->fds.in, 0) < 0)
						return (perror("bashy: dup2:"), -1);
					close(root->right->fds.in); // Close the original input file descriptor
					execute_tree(global, root->right); // Execute the right node command
					return (close_pipe(pipe), SUCCESS); // Close the pipe and return success
				}
			}
		}
	}

	// If no redirection, use the pipe's read end
	if (dup2(pipe[0], 0) < 0)
		return (perror("bashy: dup2:"), -1); // Handle duplication error
	close_pipe(global->pipe); // Close the pipe
	execute_tree(global, root->right); // Execute the right node command
	return (SUCCESS); // Return success
}

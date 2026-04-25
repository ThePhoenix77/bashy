#include "bashy.h"

// Execute the left child of a command in the syntax tree
int	execute_left(t_global *global, t_tree *root, int *pipe)
{
	// Check if there are redirection operations for the left child
	if (root->left->redir)
	{
		// Handle the redirection and update the file descriptors
		if (handle_redirection(root->left->redir, &root->left->fds) == SUCCESS)
		{
			// If there is an input file descriptor, duplicate it to stdin
			if (root->left->fds.in != -1)
			{
				if (dup2(root->left->fds.in, 0) < 0)
					return (perror("bashy: dup2:"), -1);
				close(root->left->fds.in); // Close the original fd after duplication
			}
			// If there is an output file descriptor, duplicate it to stdout
			if (root->left->fds.out != -1)
			{
				if (dup2(root->left->fds.out, 1) < 0)
					return (perror("bashy: dup2:"), -1);
				(close(root->left->fds.out), execute_tree(global, root->left)); // Execute the left child
				return (close_pipe(pipe), SUCCESS); // Close the pipe and return success
			}
		}
	}
	// If no redirection, duplicate the write end of the pipe to stdout
	if (dup2(pipe[1], 1) < 0)
		return (perror("bashy: dup2:"), -1);
	close(pipe[1]); // Close the write end of the pipe
	close(pipe[0]); // Close the read end of the pipe
	execute_tree(global, root->left); // Execute the left child
	return (SUCCESS); // Return success
}

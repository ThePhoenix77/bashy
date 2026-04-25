#include "bashy.h"

// Handle redirection for a single node in the execution list
bool	redir_one_node(t_global *global, t_exc_list **exc_node)
{
	t_redir	*redir;

	redir = (*exc_node)->redir; // Get the redirection info from the execution node
	if (handle_redirection(redir, &(*exc_node)->fd) == SUCCESS) // Handle redirection
	{
		if ((*exc_node)->fd.in != -1) // If there is an input redirection
		{
			global->save_fd.in = ft_dup(global->save_fd.in, 0); // Save current input FD
			if (global->save_fd.in < 0)
				return (FAILURE); // Handle failure in duplication
			if (ft_dup2((*exc_node)->fd.in, 0) == FAILURE) // Redirect input
				return (reset_sett(&global->save_fd), FAILURE);
		}
		if ((*exc_node)->fd.out != -1) // If there is an output redirection
		{
			global->save_fd.out = ft_dup(global->save_fd.out, 1); // Save current output FD
			if (global->save_fd.out < 0)
				return (FAILURE); // Handle failure in duplication
			if (ft_dup2((*exc_node)->fd.out, 1) == FAILURE) // Redirect output
				return (reset_sett(&global->save_fd), FAILURE);
		}
	}
	return (SUCCESS); // Successfully handled redirection
}

// Execute a built-in command if the execution node type is BUILTIN
bool	execute_one_builtin(t_global *global,
		char **cmd_args, t_exc_list *exc_node)
{
	if (exc_node->type == BUILTIN) // Check if the node type is BUILTIN
	{
		if (exc_node->redir) // If there are redirections
			if (redir_one_node(global, &exc_node) == FAILURE)
				return (FAILURE); // Handle redirection failure
		// Execute the built-in command
		check_builtins(global, cmd_args);
		reset_sett(&global->save_fd); // Reset saved file descriptors
		return (SUCCESS);
	}
	return (FAILURE); // Node is not a built-in
}

// Handle redirection for the main command once
void	handle_redir_once(t_global *global)
{
	if (global->root->redir) // Check if there are redirections
	{
		if (handle_redirection(global->root->redir,
				&global->root->fd) == SUCCESS) // Handle redirection
		{
			if (global->root->fd.in > 0) // If there is a valid input FD
			{
				if (dup2(global->root->fd.in, 0) < 0) // Redirect input
					perror("bashy : dup2:"); // Handle error
				close(global->root->fd.in); // Close the input FD
			}
			if (global->root->fd.out > 0) // If there is a valid output FD
			{
				if (dup2(global->root->fd.out, 1) < 0) // Redirect output
					perror("bashy : dup2:"); // Handle error
				close(global->root->fd.out); // Close the output FD
			}
		}
		else
			exit (1); // Exit on failure to handle redirection
	}
}

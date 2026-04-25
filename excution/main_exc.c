#include "bashy.h"

// Join consecutive nodes of type ENV or WORD into global->content
bool	join_nodes(t_global *global, t_lst **node)
{
	char	*save;

	if (((*node)->type == ENV || (*node)->type == WORD)
		&& ((*node)->next && ((*node)->next->type == ENV
				|| (*node)->next->type == WORD)))
	{
		while (*node && ((*node)->type == ENV || (*node)->type == WORD))
		{
			save = ft_strjoin2(global->content, (*node)->content);
			if (!save)
				malloc_failed(global); // Handle memory allocation failure
			if (global->content)
				free(global->content); // Free previous content
			global->content = save; // Update global content
			*node = (*node)->next; // Move to the next node
		}
		global->type = WORD; // Set the type to WORD
		add_pre_list(global); // Add the joined content to the list
		return (SUCCESS);
	}
	return (FAILURE); // No nodes joined
}

// Prepare the list by processing each node in the global list
void	prepere_list(t_global *global)
{
	global->tmp = global->head; // Start from the head of the list
	while (global->tmp)
	{
		if (global->tmp->type == ENV_SPL)
		{
			split_env(global, global->tmp); // Split the environment variable
			if (join_cont(global, global->tmp) == 3)
				global->tmp = global->tmp->next; // Move to the next node if needed
		}
		else if (global->tmp->type != WHITE_SPACE && global->tmp->type != SKIP)
		{
			if (join_nodes(global, &global->tmp) == SUCCESS)
				continue; // If nodes were joined, continue to the next iteration
			// Otherwise, set content and state from the current node
			global->content = ft_strdup(global->tmp->content);
			if (global->type == ENV)
				global->type = WORD; // Update type if it was ENV
			global->type = global->tmp->type; // Update type
			global->state = global->tmp->state; // Update state
			add_pre_list(global); // Add the current content to the list
		}
		global->tmp = global->tmp->next; // Move to the next node
	}
}

// Print an error message when a command is not found
void	cmd_msg(char *cmd)
{
	ft_putstr_fd("bashy: ", 2); // Print the prefix
	ft_putstr_fd(cmd, 2); // Print the command
	ft_putstr_fd(ERR_CMD_NF, 2); // Print the command not found error
}

// Execute the command or pipeline based on the number of commands
bool	main_exc_p2(t_global *global)
{
	global->exc_size = exc_list_size(global->root); // Get the number of executable commands
	if (global->exc_size == 1)
	{
		execute_one_cmd(global); // Execute the single command
		return (FAILURE); // Return failure as it is executed
	}
	if (global->exc_size == 0)
		return (FAILURE); // No commands to execute
	global->root_tree = build_tree(global, &global->root); // Build the command execution tree
	execute_tree(global, global->root_tree); // Execute the command tree
	return (SUCCESS); // Return success
}

// Main execution function for processing commands and managing execution flow
void	main_exc(t_global *global)
{
	t_exc_list	*exc_tmp;

	prepere_list(global); // Prepare the command list
	bulid_list_exc(global); // Build the execution list
	check_builtins_cmds(global->root); // Check for built-in commands
	env_list_2d_array(global); // Prepare environment variables for execution
	exc_tmp = global->root; // Start with the root of the execution list
	while (exc_tmp)
	{
		if (exc_tmp->redir) // If there are redirections
		{
			if (check_doubel_redir(global, &exc_tmp->redir) == FAILURE) // Check for double redirection
			{
				printf("FAILIER\n"); // Print failure message
				global->exit_status = 1; // Set exit status to 1
				exit(1); // Exit the program
			}
		}
		exc_tmp = exc_tmp->next; // Move to the next command
	}
	main_exc_p2(global); // Execute the commands
}

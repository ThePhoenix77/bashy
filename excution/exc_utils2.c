#include "bashy.h"

// Count the number of arguments in the cmd_args linked list
int	args_arr_size(t_cmd_args *cmd_args)
{
	int			arg_count; // Counter for the number of arguments
	t_cmd_args	*temp; // Temporary pointer to traverse the list

	arg_count = 0; // Initialize argument count to zero
	temp = cmd_args; // Start with the head of the list
	while (temp) // Traverse the linked list
	{
		arg_count++; // Increment count for each argument
		temp = temp->next; // Move to the next argument
	}
	return (arg_count); // Return the total count
}

// Add command arguments from the cmd_args list to the cmd_ars array
void	add_cmd_args(t_global *global, char **cmd_ars, t_cmd_args *cmd_args)
{
	int	i; // Index for cmd_ars

	i = 1; // Start at index 1 (index 0 is reserved)
	if (cmd_ars) // Ensure cmd_ars is not NULL
	{
		while (cmd_args) // Iterate through the cmd_args linked list
		{
			cmd_ars[i] = ft_strdup(cmd_args->content); // Duplicate argument content
			if (!cmd_ars[i]) // Check for successful duplication
			{
				perror("bashy: malloc"); // Print error if malloc fails
				free_2d_array(&cmd_ars); // Free allocated cmd_ars on error
				malloc_failed(global); // Handle memory allocation failure
			}
			i++; // Increment index
			cmd_args = cmd_args->next; // Move to the next argument
		}
	}
	cmd_ars[i] = NULL; // Set the last element to NULL to terminate the array
}

// Create an array of arguments from the cmd_args linked list
char	**get_args_arr(t_global *global, t_cmd_args *cmd_args, t_type_node type)
{
	char	**args_arr; // Array to hold command arguments

	// Allocate memory for the arguments array
	args_arr = (char **)malloc((args_arr_size(cmd_args) + 1) * sizeof(char *));
	if (!args_arr) // Check for successful allocation
		malloc_failed(global); // Handle memory allocation failure

	// Determine the first argument based on content and type
	if (cmd_args->content[0] == '\0')
		args_arr[0] = ft_strdup("\x02"); // Handle empty command case
	else if (type == BUILTIN)
		args_arr[0] = ft_strdup(cmd_args->content); // Directly assign built-in command
	else
		args_arr[0] = get_path(cmd_args->content, global); // Get the command path

	// If path retrieval fails, handle the error
	if (!args_arr[0] && cmd_args->content[0] != '\0')
		args_arr[0] = ft_strjoin2("\x02", cmd_args->content); // Fallback to join
	if (!args_arr[0]) // If still NULL, handle allocation failure
		(free(args_arr), malloc_failed(global));
	
	add_cmd_args(global, args_arr, cmd_args->next); // Add remaining command arguments
	return (args_arr); // Return the populated arguments array
}

// Print an error message for a command not found and exit with status 127
void	ft_cmd_err(char *cmd)
{
	ft_putstr_fd("bashy: ", 2); // Print prefix for error message
	ft_putstr_fd(cmd, 2); // Print the command that failed
	ft_putstr_fd(ERR_CMD_NF, 2); // Print "command not found" error
	exit(127); // Exit with command not found status
}

// Check and categorize commands as built-ins or regular commands
void	check_builtins_cmds(t_exc_list *exc_node)
{
	while (exc_node) // Iterate through execution nodes
	{
		if (exc_node->cmd_args) // Check if the node has command arguments
		{
			// Compare command against built-in commands
			if (ft_strcmp(exc_node->cmd_args->content, "echo") == SUCCESS)
				exc_node->type = BUILTIN; // Mark as built-in
			else if (ft_strcmp(exc_node->cmd_args->content, "cd") == SUCCESS)
				exc_node->type = BUILTIN;
			else if (ft_strcmp(exc_node->cmd_args->content, "pwd") == SUCCESS)
				exc_node->type = BUILTIN;
			else if (ft_strcmp(exc_node->cmd_args->content, "export") == 1)
				exc_node->type = BUILTIN;
			else if (ft_strcmp(exc_node->cmd_args->content, "unset") == SUCCESS)
				exc_node->type = BUILTIN;
			else if (ft_strcmp(exc_node->cmd_args->content, "env") == SUCCESS)
				exc_node->type = BUILTIN;
			else if (ft_strcmp(exc_node->cmd_args->content, "exit") == SUCCESS)
				exc_node->type = BUILTIN;
			else
				exc_node->type = CMD; // Mark as a regular command
		}
		exc_node = exc_node->next; // Move to the next execution node
	}
}

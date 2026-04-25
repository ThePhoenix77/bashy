#include "bashy.h"

/**
 * unset_is_valid - Validates if the given argument is a valid environment
 *                  variable name for the 'unset' command.
 * @str: The argument string representing the environment variable to unset.
 * 
 * Returns 1 if the identifier is valid, 0 otherwise.
 * 
 * Valid identifiers must start with a letter or underscore and can contain
 * alphanumeric characters or underscores. This ensures compatibility with
 * standard environment variable naming conventions.
 */
int	unset_is_valid(char *str)
{
	int	i;

	i = 1;
	// Ensure the string is non-null and starts with a letter or underscore
	if (!str || (!ft_isalpha(str[0]) && str[0] != '_'))
		return (0);

	// Check the rest of the string for alphanumeric or underscore characters
	while (str[i])
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (0);
		i++;
	}
	return (1); // The identifier is valid 
}

/**
 * remove_env_node - Removes a node from the environment linked list.
 * @global: Pointer to the global state structure which holds the env list.
 * @node: The environment list node to be removed.
 * 
 * This function handles the removal of an environment variable node from
 * the doubly-linked list by adjusting the pointers of neighboring nodes
 * and freeing the memory associated with the node.
 */
void	remove_env_node(t_global *global, t_env_list *node)
{
	// If the node is not the first, adjust the previous node's next pointer
	if (node->prev)
		node->prev->next = node->next;
	else
		global->env_list = node->next; // If it's the first node, update the head of the list

	// If the node is not the last, adjust the next node's previous pointer
	if (node->next)
		node->next->prev = node->prev;

	// Free the memory for the node's content and the node itself
	free(node->content);
	free(node);
}

/**
 * ft_unset - Implements the 'unset' shell command, which removes environment
 *            variables from the environment list.
 * @global: Pointer to the global state structure which holds the env list.
 * @args: Array of arguments passed to the 'unset' command.
 * 
 * This function loops through the provided arguments, checking if each one is
 * a valid identifier. If it is, it attempts to find and remove the corresponding
 * environment variable from the linked list. If an invalid identifier is provided,
 * an error message is printed, and the exit status is updated accordingly.
 */
void	ft_unset(t_global *global, char **args)
{
	t_env_list	*env_var;
	int			i;

	i = 1;
	global->exit_status = 0; // Initialize exit status to success

	// Loop through all provided arguments
	while (args[i])
	{
		// Validate the argument as a valid identifier
		if (!unset_is_valid(args[i]))
		{
			// If invalid, print an error message to stderr and update the exit status
			ft_putstr_fd("unset: `", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			global->exit_status = 1; // Set exit status to indicate an error
		}
		else
		{
			// If valid, find the corresponding environment variable
			env_var = find_var(global->env_list, args[i]);
			if (env_var) // If the variable exists, remove it
				remove_env_node(global, env_var);
		}
		i++; // Move to the next argument
	}
}

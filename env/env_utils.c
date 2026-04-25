#include "bashy.h"

/**
 * new_env_node - Creates a new environment variable node.
 * @content: The content of the environment variable (e.g., "KEY=value").
 * 
 * This function allocates memory for a new `t_env_list` node and initializes
 * its content, `next`, and `prev` pointers. It returns a pointer to the new
 * node, or NULL if memory allocation fails.
 * 
 * Return: A pointer to the newly created environment node, or NULL on failure.
 */
t_env_list	*new_env_node(char *content)
{
	t_env_list	*new;

	// Allocate memory for the new node
	new = (t_env_list *)malloc(sizeof(t_env_list));
	if (!new)
		return (NULL);  // Return NULL if memory allocation fails

	// Initialize the content and set next and prev pointers to NULL
	new->content = content;
	new->next = NULL;
	new->prev = NULL;

	return (new);
}

/**
 * last_env_node - Finds the last node in the environment list.
 * @head: Pointer to the head of the environment list.
 * 
 * This function traverses the environment list to find the last node. If the 
 * list is empty (head is NULL), it returns NULL.
 * 
 * Return: A pointer to the last node in the list, or NULL if the list is empty.
 */
t_env_list	*last_env_node(t_env_list *head)
{
	if (!head)  // Return NULL if the list is empty
		return (NULL);

	// Traverse the list to find the last node
	while (head->next)
		head = head->next;

	return (head);
}

/**
 * free_env_list - Frees all nodes in the environment variable list.
 * @head: Double pointer to the head of the environment list.
 * 
 * This function iterates through the entire environment list, freeing each node
 * and its associated content. It sets the head to NULL after the list is freed.
 */
void	free_env_list(t_env_list **head)
{
	t_env_list	*tmp;

	if (!(*head))  // If the list is empty, return immediately
		return ;

	// Traverse and free each node in the list
	while ((*head))
	{
		tmp = *head;
		*head = (*head)->next;
		free(tmp->content);  // Free the environment variable string
		free(tmp);           // Free the node itself
	}

	// Set the head to NULL after the list is cleared
	*head = NULL;
}

/**
 * add_env_back - Adds a new node to the end of the environment variable list.
 * @head: Double pointer to the head of the environment list.
 * @new: Pointer to the new node to add to the list.
 * 
 * This function appends the new environment node to the end of the list. If the 
 * list is empty, the new node becomes the head.
 */
void	add_env_back(t_env_list **head, t_env_list *new)
{
	t_env_list	*last_node;

	if (!(new))  // If the new node is NULL, do nothing
		return ;

	// If the list is empty, the new node becomes the head
	if (!(*head))
		*head = new;
	else
	{
		// Find the last node in the list
		last_node = last_env_node(*head);

		// Append the new node to the end of the list
		last_node->next = new;
		new->prev = last_node;
	}
}

/**
 * check_env - Initializes and checks the environment variables for the shell.
 * @global: Pointer to the global state structure.
 * 
 * This function checks the `env` array for existing environment variables. If 
 * the environment is empty, it calls `empty_env` to initialize a minimal set of 
 * environment variables. It also skips "OLDPWD" if present and adds the rest of
 * the environment variables to the shell's environment list. Finally, it adjusts
 * the shell level (`SHELL_LEVEL`) to maintain a proper nesting level for the shell.
 * 
 * Return: SUCCESS on successful initialization, FAILURE on error.
 */
bool	check_env(t_global *global)
{
	int			i;
	t_env_list	*new;

	i = 0;

	// Check if the environment is empty, and initialize if necessary
	if (!global->env || !global->env[0])
	{
		if (empty_env(global) == FAILURE)
			return (FAILURE);
	}
	else if (global->env || global->env[0])
	{
		// Loop through the environment variables and add them to the list
		while (global->env[i])
		{
			// Skip the "OLDPWD" variable
			if (ft_strncmp("OLDPWD=", global->env[i], 7) == 0)
				i++;

			// Create a new environment node for the variable
			new = new_env_node(ft_strdup(global->env[i]));
			if (!new || !new->content)
				return (FAILURE);

			// Add the new node to the environment list
			add_env_back(&global->env_list, new);
			i++;
		}
	}

	// Adjust the shell level (e.g., increment/decrement SHELL_LEVEL)
	if (shell_level(global) == FAILURE)
		return (FAILURE);

	return (SUCCESS);
}

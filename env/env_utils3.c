#include "bashy.h"

/**
 * env_list_size - Computes the size of the environment variable linked list.
 * @global: Pointer to the global state structure.
 * 
 * This function traverses the linked list of environment variables and counts 
 * the number of nodes. The size is used to allocate memory for a 2D array 
 * representation of the environment.
 * 
 * Return: The number of nodes (environment variables) in the list.
 */
int	env_list_size(t_global *global)
{
	int			list_size;
	t_env_list	*tmp;

	list_size = 0;
	tmp = global->env_list;

	// Traverse the environment list and count the nodes
	while (tmp)
	{
		list_size++;
		tmp = tmp->next;
	}

	return (list_size);
}

/**
 * env_list_2d_array - Converts the environment variable linked list into a 2D array.
 * @global: Pointer to the global state structure.
 * 
 * This function converts the linked list of environment variables into a 2D array
 * of strings, where each string is an environment variable in the format "KEY=VALUE".
 * It allocates memory for the array and copies the content of each node into the array.
 * The last element of the array is set to NULL to mark the end.
 */
void	env_list_2d_array(t_global *global)
{
	int			list_size;
	t_env_list	*tmp;
	int			i;

	// Get the size of the environment list to allocate memory for the 2D array
	list_size = env_list_size(global);
	tmp = global->env_list;

	// Allocate memory for the 2D array (list_size + 1 for the NULL terminator)
	global->myenv = (char **)malloc(sizeof(char *) * (list_size + 1));
	if (!global->myenv)
		malloc_failed(global);  // Handle memory allocation failure

	i = 0;
	// Traverse the environment list and copy the content into the 2D array
	while (tmp)
	{
		// Duplicate the content of the current node and store it in the array
		global->myenv[i] = ft_strdup(tmp->content);
		if (!global->myenv[i])
			malloc_failed(global);  // Handle memory allocation failure

		i++;
		tmp = tmp->next;
	}

	// Set the last element of the array to NULL
	global->myenv[list_size] = NULL;
}

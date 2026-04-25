#include "bashy.h"

/**
 * get_var_env - Retrieves an environment variable node by its name.
 * @global: Pointer to the global state structure.
 * @var: The name of the environment variable to find (e.g., "PATH").
 * 
 * This function searches through the environment list for a variable that matches
 * the provided name. It uses `ft_strlen_un_char` to determine the length of the 
 * variable name (up to the `=` sign) and compares it to the content of each node.
 * 
 * Return: A pointer to the matching environment node, or NULL if not found.
 */
t_env_list	*get_var_env(t_global *global, char *var)
{
	t_env_list	*tmp;
	int			len;

	tmp = global->env_list;

	// Calculate the length of the variable name (including '=')
	len = ft_strlen_un_char(var, '=') + 1;

	// Traverse the environment list to find the variable
	while (tmp)
	{
		// Compare the variable name (var) with the content of the current node
		if (ft_strncmp(var, tmp->content, len) == 0)
			return (tmp);
		tmp = tmp->next;
	}

	return (NULL);  // Return NULL if the variable is not found
}

/**
 * update_env - Updates the value of an existing environment variable.
 * @env: Double pointer to the environment node to update.
 * @new_val: The new value to assign to the environment variable (e.g., "/usr/bin").
 * 
 * This function updates an environment variable by creating a new string that 
 * combines the variable name (before the `=`) with the new value. The old content 
 * is freed, and the new string is assigned to the node. The function handles memory
 * allocation and ensures proper cleanup in case of failure.
 * 
 * Return: SUCCESS on successful update, FAILURE on memory allocation error.
 */
bool	update_env(t_env_list **env, char *new_val)
{
	int		len;
	int		len_add;
	char	*res;
	char	*tmp;

	// Calculate lengths: len for the variable name, len_add for the new value
	len = ft_strlen_un_char((*env)->content, '=') + 1;
	len_add = ft_strlen(new_val);

	// Allocate memory for the new content (variable name + new value + null terminator)
	res = malloc(sizeof(char) * (len + len_add + 1));
	if (!res)
		return (FAILURE);  // Return FAILURE if memory allocation fails

	// Copy the variable name (up to the '=')
	ft_strlcpy(res, (*env)->content, len + 1);

	// Save the original pointer for later cleanup
	tmp = res;

	// Concatenate the new value to the result (res)
	res = ft_strjoin(res, new_val);
	if (!res)
		return (FAILURE);  // Return FAILURE if memory allocation fails

	// Free the old content and temporary pointers
	free((*env)->content);
	free(new_val);
	free(tmp);

	// Assign the newly created string to the environment node
	(*env)->content = res;
	res = NULL;  // Clear the temporary pointer

	return (SUCCESS);
}

/**
 * empty_env - Initializes the environment list with default values when the
 *             environment is empty.
 * @global: Pointer to the global state structure.
 * 
 * This function is called when the environment is empty (i.e., no existing
 * environment variables). It sets up a minimal set of default environment
 * variables, including "PWD" (current working directory), "SHLVL=0" (shell
 * level), and "_=./bashy" (last executed command). These variables are
 * added to the environment list.
 * 
 * Return: SUCCESS on successful initialization, FAILURE on error.
 */
bool	empty_env(t_global *global)
{
	char		*curr_path;
	t_env_list	*new;

	// Get the current working directory
	curr_path = getcwd(NULL, 0);
	if (curr_path == NULL)
		return (FAILURE);  // Return FAILURE if getcwd fails

	// Create and add the PWD (current directory) environment variable
	new = new_env_node(ft_strjoin("PWD=", curr_path));
	if (!new || !new->content)
	{
		free(curr_path);  // Free the current path in case of failure
		return (FAILURE);
	}
	free(curr_path);  // Free the current path after use
	add_env_back(&global->env_list, new);  // Add the PWD node to the environment list

	// Create and add the SHLVL (shell level) environment variable
	new = new_env_node(ft_strdup("SHLVL=0"));
	if (!new || !new->content)
		return (FAILURE);
	add_env_back(&global->env_list, new);  // Add the SHLVL node to the environment list

	// Create and add the last executed command variable ("_")
	new = new_env_node(ft_strdup("_=./bashy"));
	if (!new || !new->content)
		return (FAILURE);
	add_env_back(&global->env_list, new);  // Add the "_" node to the environment list

	return (SUCCESS);
}

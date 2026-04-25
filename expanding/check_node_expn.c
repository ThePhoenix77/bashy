#include "bashy.h"

/**
 * find_dollar - Checks if the content of the node contains a valid `$` expansion.
 * @node: Pointer to the current token node.
 * 
 * This function scans the content of the token node to determine if it contains 
 * a dollar sign (`$`) followed by a valid character for expansion (e.g., a valid 
 * environment variable or special expansion like `$?`).
 * 
 * Return: SUCCESS if a valid `$` expansion is found, FAILURE otherwise.
 */
bool	find_dollar(t_lst *node)
{
	int	i;

	i = 0;

	// Traverse the content of the node to look for '$' followed by a valid character
	while (node->content[i])
	{
		if (node->content[i] == '$' && ft_is_valid_expnd(node->content[i + 1]))
			return (SUCCESS);
		i++;
	}
	return (FAILURE);
}

/**
 * tilde_exp - Expands the tilde (`~`) to the user's home directory.
 * @global: Pointer to the global state structure.
 * @node: Double pointer to the current token node.
 * 
 * This function expands the tilde character (`~`) in the token content to the 
 * user's home directory as specified in the `HOME` environment variable. It 
 * handles cases where `~` is followed by a `/`, and properly concatenates paths.
 * 
 * Return: SUCCESS on successful expansion, FAILURE if `HOME` is not set.
 */
int	tilde_exp(t_global *global, t_lst **node)
{
	t_env_list	*tmp;
	int			len;
	char		*res;
	char		*save;

	// Retrieve the value of the HOME environment variable
	tmp = get_var_env(global, "HOME=");
	if (!tmp)
		return (FAILURE);  // Return FAILURE if HOME is not set

	// Duplicate the HOME directory path for use
	res = ft_strdup(tmp->content);
	if (!res)
		malloc_failed(global);

	// Skip the "HOME=" part in the content and retain only the directory path
	len = ft_strlen_un_char(res, '=') + 1;
	update_line(&res, len, global);

	// If the tilde is followed by a '/', append a slash to the result
	if ((*node)->content[0] == '/')
	{
		save = res;
		res = ft_strjoin2(res, "/");  // Concatenate a slash to the home directory
		free(save);
		if (!res)
			malloc_failed(global);

		// Update the node content to remove the leading '/'
		update_line(&(*node)->content, 1, global);
	}

	// Concatenate the expanded home directory with the remaining content
	save = (*node)->content;
	(*node)->content = ft_strjoin2(res, (*node)->content);

	// Free temporary memory and return success
	return (free(save), free(res), SUCCESS);
}

/**
 * check_expand - Processes and expands environment variables and tildes in the token list.
 * @global: Pointer to the global state structure.
 * 
 * This function traverses the token list and performs expansions on nodes that 
 * are marked as `WORD` and are in either the `IN_DQUOTE` or `GENERAL` state. It 
 * handles tilde expansion (`~`), replacing it with the home directory, and also 
 * detects and marks nodes for environment variable expansion when `$` is found.
 * 
 * Return: SUCCESS on successful expansion processing, FAILURE on error (e.g., 
 *         if `HOME` is not set during tilde expansion).
 */
bool	check_expand(t_global *global)
{
	t_lst	*tmp;

	tmp = global->head;

	// Traverse the token list and check each node for expansion
	while (tmp)
	{
		// Check if the token is a WORD and is either in double quotes or general state
		if ((tmp->type == WORD && tmp->state == IN_DQUOTE)
			|| (tmp->type == WORD && tmp->state == GENERAL))
		{
			// Handle tilde expansion for tokens that start with `~` followed by `/` or nothing
			if (tmp->type == WORD && tmp->state == GENERAL
				&& (tmp->content[0] == '~'
					&& (!tmp->content[1] || tmp->content[1] == '/')))
			{
				update_line(&tmp->content, 1, global);  // Remove the tilde
				if (tilde_exp(global, &tmp) == FAILURE)
					return (ft_putstr_fd("Home not set\n", 2), FAILURE);
			}

			// Check if the content contains a `$` for environment variable expansion
			if (find_dollar(tmp) == SUCCESS)
				tmp->type = ENV;  // Mark the token type as an environment variable
		}

		// Move to the next token in the list
		tmp = tmp->next;
	}

	return (SUCCESS);
}

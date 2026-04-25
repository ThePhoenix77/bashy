#include "bashy.h"

/**
 * check_af_sp - Checks if there are only spaces after a redirection symbol.
 * @inp: Input string to check (typically the content after a redirection).
 * 
 * This function iterates through the input string, skipping spaces. 
 * If only spaces are found after the redirection operator, it returns SUCCESS.
 * If non-whitespace characters are found, it returns FAILURE.
 * 
 * Return: SUCCESS if only spaces or the end of the string is found after spaces,
 *         FAILURE if non-space characters are found.
 */
bool	check_af_sp(char *inp)
{
	int	i;

	i = 0;
	// Loop through the input string
	while (inp[i])
	{
		// Check for spaces
		if (inp[i] == ' ')
		{
			// Skip consecutive spaces
			while (inp[i] && inp[i] == ' ')
				i++;

			// If the end of the string is reached after spaces, return SUCCESS
			if (inp[i] == '\0')
				return (SUCCESS);

			// If any other character is found after spaces, return FAILURE
			return (FAILURE);
		}
		i++;
	}
	// If no spaces or invalid characters were found, return SUCCESS
	return (SUCCESS);
}

/**
 * set_error - Sets an error message and type for an ambiguous redirection.
 * @global: Pointer to the global state structure.
 * @node: Pointer to the node in the token list where the error occurred.
 * 
 * This function handles ambiguous redirection errors, updating the content of 
 * the token node with an error message ("bashy: ambiguous redirect") and 
 * setting its type to ERROR_DIS.
 */
void	set_error(t_global *global, t_lst *node)
{
	// Free existing content in the node
	if (node->content)
		free(node->content);

	// Set the content to an error message indicating ambiguous redirect
	node->content = ft_strdup("bashy: ambiguous redirect");
	if (!node->content)
		malloc_failed(global); // Handle memory allocation failure

	// Mark the node type as ERROR_DIS to indicate a redirection error
	node->type = ERROR_DIS;
}

/**
 * check_redir - Checks redirection syntax and detects ambiguous redirects.
 * @global: Pointer to the global state structure containing the token list.
 * 
 * This function iterates through the token list, checking for redirection
 * operators (>, >>, <). It ensures that proper tokens follow the redirection 
 * operators and flags any errors (e.g., ambiguous redirects caused by 
 * environment variables or invalid tokens). If an error is found, it updates 
 * the node with an error message.
 */
void	check_redir(t_global *global)
{
	t_lst	*tmp;

	tmp = global->head; // Start with the head of the token list

	// Loop through each token in the list
	while (tmp)
	{
		// Check if the current token is a redirection operator (>, >>, <)
		if (tmp->type == REDIR_IN || tmp->type == REDIR_OUT || tmp->type == DREDIR_OUT)
		{
			// If the next token is a whitespace token, skip to the next relevant token
			if (tmp->next->type == WHITE_SPACE)
				tmp = tmp->next->next;
			else
				tmp = tmp->next;

			// Check if the next token is an environment variable (ENV or ENV_SPL)
			if (tmp->state == GENERAL && (tmp->type == ENV_SPL || tmp->type == ENV))
			{
				// Check if the environment variable is ambiguous (contains spaces or is empty)
				if (check_af_sp(tmp->content) == FAILURE || !tmp->content || !tmp->content[0])
					set_error(global, tmp); // Set the error if the content is invalid
			}
		}

		// Move to the next token in the list
		tmp = tmp->next;
	}
}

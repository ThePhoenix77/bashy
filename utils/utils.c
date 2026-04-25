#include "bashy.h"

// Initialize global structure with default values and environment variables
void	init_global(t_global *global, char **env)
{
	global->env = env; // Set the environment variable pointer
	global->here_doc_num = 0;
	global->exc_size = 0;
	global->list_len = 0;
	global->exit_status = 0;
	global->state = -1;
	global->type = -1;
	global->root = NULL;
	global->root_tree = NULL;
	global->head = NULL;
	global->myenv = NULL;
	global->content = NULL;
	global->env_list = NULL;
	global->pre_head = NULL;
	global->exp_head = NULL;
	global->save_fd.in = -1; // Initialize file descriptors
	global->save_fd.out = -1;
}

// Check for valid input in the global structure
int	check_valid_in(t_global *global)
{
	if (!global->line_input)
		return (FAILURE); // Return failure if line input is NULL
	if (global->line_input[0] == '\0') // Check for empty line
	{
		free(global->line_input); // Free allocated line input
		return (SUCCESS);
	}
	return (2); // Return 2 if valid input is present
}

// Find the length of a string until a specified character is encountered
int	ft_strlen_un_char(char *str, char c)
{
	int	i;

	i = 0;
	if (!str)
		return (-1); // Return -1 if the string is NULL
	while (str[i])
	{
		if (str[i] == c) // Return the index if character c is found
			return (i);
		i++;
	}
	return (-2); // Return -2 if character c is not found
}

// Find the length of a string until a specified delimiter is encountered
int	ft_strlen_un_del(char *sep, char *str)
{
	int	i;
	int	j;

	i = 0;
	if (!str)
		return (-1); // Return -1 if the string is NULL
	if (ft_strchr(sep, str[0])) // Check if the first character is a delimiter
		return (1);
	while (str[i]) // Iterate through the string
	{
		j = 0;
		while (sep[j]) // Iterate through the delimiters
		{
			if (sep[j] == str[i]) // Return index if delimiter is found
				return (i);
			j++;
		}
		i++;
	}
	return (0); // Return 0 if no delimiter is found
}

// Compare two strings for equality
int	ft_strcmp(char *s1, char *s2)
{
	int		i;

	i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i]) // Compare characters until mismatch or end of string
		i++;
	if (s1[i] == '\0' && s2[i] == '\0') // Return SUCCESS if both strings are equal
		return (SUCCESS);
	return (FAILURE); // Return FAILURE if they are not equal
}

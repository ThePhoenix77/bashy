#include "bashy.h"

/**
 * extract_key - Extracts the key portion of an environment variable.
 * @arg: Full argument string (e.g., "KEY=value" or "KEY+=").
 * @plus_equal: Pointer to the "+=" part in the argument string.
 * 
 * Returns a dynamically allocated string containing the key (substring from
 * the beginning of @arg up to @plus_equal).
 */
char	*extract_key(char *arg, char *plus_equal)
{
	int	key_length;

	key_length = plus_equal - arg; // Calculate key length as distance between start of arg and "+="
	return (ft_strndup(arg, key_length)); // Return a copy of the key
}

/**
 * concat_env_value - Concatenates the current value of an environment variable with the new one.
 * @env_var: The environment variable (t_env_list) to update.
 * @value: The new value to be appended.
 * @key_length: Length of the key in the current environment variable.
 * 
 * Returns a new dynamically allocated string containing the concatenated value.
 */
char	*concat_env_value(t_env_list *env_var, char *value, int key_length)
{
	return (ft_strjoin(env_var->content + key_length + 1, value)); 
	// Skips the key and '=' to access the current value, then concatenates it with the new value
}

/**
 * plus_checker - Handles variables with "+=" syntax for appending values to existing variables.
 * @global: Pointer to global state structure, which holds environment variables.
 * @arg: Argument string from the export command.
 * 
 * Checks if the argument contains "+=" and appropriately updates the environment 
 * variable by appending the new value to the existing one.
 */
void	plus_checker(t_global *global, char *arg)
{
	char		*plus_equal;
	char		*key;
	char		*value;
	char		*new_value;
	t_env_list	*env_var;

	// Search for "+=" in the argument string
	plus_equal = ft_strnstr(arg, "+=", ft_strlen(arg));
	if (!plus_equal) // No "+=" found, nothing to process
		return ;
	key = extract_key(arg, plus_equal); // Extract the key before "+="
	value = ft_strdup(plus_equal + 2); // Extract the new value after "+="
	env_var = find_var(global->env_list, key); // Look for the existing environment variable
	if (env_var && value) // If variable exists and there's a new value
		new_value = concat_env_value(env_var, value, plus_equal - arg); // Append new value to existing one
	else
		new_value = value; // No existing variable, use the new value directly

	// Add or update the environment variable, flag it as hidden if needed
	add_or_update_env_var(global, key, new_value, HIDE);

	// Free allocated memory
	free(key);
	free(value);
	if (env_var && new_value)
		free(new_value);
}

/**
 * is_valid_identifier - Checks if a given argument is a valid environment variable identifier.
 * @arg: Argument string to validate (e.g., "VAR=value").
 * 
 * Returns 1 if the identifier is valid, 0 otherwise.
 * 
 * Valid identifiers start with a letter or underscore, followed by alphanumeric
 * characters or underscores, and do not contain '=' in the identifier portion.
 */
int	is_valid_identifier(char *arg)
{
	int	i;

	i = 1;
	// First character must be a letter or underscore
	if (!ft_isalpha(arg[0]) && arg[0] != '_')
		return (0);

	// Remaining characters must be alphanumeric or underscore, stopping at '='
	while (arg[i] && arg[i] != '=')
	{
		if (!ft_isalnum(arg[i]) && arg[i] != '_')
			return (0);
		i++;
	}
	return (1); // Identifier is valid
}

/**
 * process_export_cmd - Handles the core logic of the 'export' command with arguments.
 * @global: Pointer to global state structure, which holds environment variables.
 * @args: List of arguments passed to the 'export' command.
 * 
 * Iterates over the arguments and validates each one as a valid identifier. If the argument
 * contains an '=' symbol, the variable is added or updated; if not, the variable is just declared.
 */
void	process_export_cmd(t_global *global, char **args)
{
	int		i;
	char	*equal_pos;

	i = 1;
	while (args[i])
	{
		// Check if the argument is a valid identifier or starts with '='
		if (!is_valid_identifier(args[i]) || args[i][0] == '=')
		{
			// Invalid identifier, print an error message to stderr
			ft_putstr_fd("bash: export: `", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			global->exit_status = 1; // Set error status
		}
		else
		{
			// Check if argument contains '=' and split key/value
			equal_pos = ft_strchr(args[i], '=');
			if (equal_pos) 
			{
				*equal_pos = '\0'; // Temporarily split the string into key/value
				add_or_update_env_var(global, args[i], equal_pos + 1, 0); // Add/update the variable
			}
			else
				add_or_update_env_var(global, args[i], NULL, 0); // Declare variable without value
		}
		i++;
	}
}

/**
 * ft_export - Entry point for the 'export' command.
 * @global: Pointer to global state structure, which holds environment variables.
 * @args: List of arguments passed to the 'export' command.
 * 
 * Handles the 'export' command by either exporting variables or printing all
 * environment variables if no arguments are passed.
 */
void	ft_export(t_global *global, char **args)
{
	global->exit_status = 0;

	// If no arguments are passed, list all exported variables
	if (!args[1])
		export_without_args(global);
	// Handle invalid variable name
	else if ((name_checker(args[1])))
	{
		ft_putstr_fd("export: Invalid variable name.\n", 2);
		global->exit_status = 1;
	}
	else
		process_export_cmd(global, args); // Process arguments and update env vars
	global->exit_status = 0;
}

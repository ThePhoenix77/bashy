#include "bashy.h"

/**
 * ft_atoi_shlvl - Converts a string to a long integer for the SHLVL value.
 * @str: The string containing the number to be converted.
 * 
 * This function works similarly to `ft_atoi`, but with additional checks for 
 * invalid characters and an overflow safeguard for out-of-range values. It 
 * ensures that only valid numeric strings are processed, skipping leading 
 * whitespace and handling signs ('+' or '-').
 * 
 * Return: The converted number, or LONG_MAX if an invalid or out-of-range value 
 *         is encountered.
 */
long	ft_atoi_shlvl(const char *str)
{
	long	num;
	int		sign;

	num = 0;
	sign = 1;

	// Skip leading whitespace
	while (ft_isspace2(*str))
		str++;

	// Handle sign
	if (*str == '-' || *str == '+')
		sign = 1 - 2 * (*str++ == '-');

	// Convert the string to a number, digit by digit
	while (ft_isdigit(*str))
		num = num * 10 + *str++ - '0';

	// Return LONG_MAX for any invalid signs after number conversion
	if (ft_issign(*str))
		return (LONG_MAX);

	return (num * sign);
}

/**
 * check_shell_level - Validates and adjusts the SHLVL value.
 * @node: Double pointer to the environment variable node (SHLVL).
 * @num: The current numeric value of SHLVL.
 * 
 * This function adjusts the SHLVL value based on certain conditions:
 * - If SHLVL is 999, the shell level is reset (empty value).
 * - If SHLVL exceeds 1000, it is clamped and reset to 1 with a warning.
 * - If SHLVL is negative, it is reset to 0.
 * 
 * Return: The adjusted SHLVL value, -1 for memory allocation failure, or -2 for 
 *         the special case of resetting SHLVL to an empty value.
 */
int	check_shell_level(t_env_list **node, int num)
{
	if (num == 999)  // Special case: SHLVL at 999 should be reset
	{
		free((*node)->content);
		(*node)->content = ft_strdup("SHLVL=");
		if (!(*node)->content)
			return (-1);  // Memory allocation failure
		return (-2);      // Indicate SHLVL has been reset
	}

	// Increment the SHLVL value
	num += 1;

	// Clamp SHLVL to 0 if negative
	if (num < 0)
		num = 0;
	else if (num > 1000)  // Reset SHLVL if it exceeds 1000 and issue a warning
	{
		ft_putstr_fd("bashy: warning: shell level (", 2);
		ft_putnbr_fd(num, 2);
		ft_putstr_fd(") too high, resetting to 1\n", 2);
		num = 1;
	}

	return (num);
}

/**
 * shell_level - Adjusts the SHLVL environment variable for the current shell.
 * @global: Pointer to the global state structure.
 * 
 * This function ensures that the SHLVL environment variable is properly 
 * initialized and incremented whenever a new shell instance is created. If the 
 * SHLVL variable doesn't exist, it is initialized to 1. The function also handles 
 * extreme cases like SHLVL being too high or being reset.
 * 
 * Return: SUCCESS if the operation is successful, FAILURE otherwise.
 */
bool	shell_level(t_global *global)
{
	t_env_list	*tmp;
	char		*ptr_num;
	int			num;
	char		*shlvl;

	// Retrieve the SHLVL variable from the environment list
	tmp = get_var_env(global, "SHLVL=");
	if (!tmp)  // If SHLVL doesn't exist, initialize it to "SHLVL=1"
	{
		shlvl = "SHLVL=1";
		ft_export(global, &shlvl);
		return (SUCCESS);
	}

	// Extract the numeric part of SHLVL (after the '=')
	ptr_num = ft_strchr(tmp->content, '=');
	num = ft_atoi_shlvl(ptr_num + 1);  // Convert the string to an integer

	// Check and adjust the shell level value
	num = check_shell_level(&tmp, num);
	if (num == -1)
		return (FAILURE);  // Memory allocation failure
	if (num == -2)
		return (SUCCESS);  // SHLVL reset, nothing more to do

	// Convert the adjusted number back to a string
	ptr_num = ft_itoa(num);
	if (!ptr_num)
		return (FAILURE);  // Memory allocation failure

	// Update the environment variable with the new SHLVL value
	if (update_env(&tmp, ptr_num) == FAILURE)
		return (FAILURE);

	return (SUCCESS);
}

#include "bashy.h"

// Converts a string to a long integer, handles overflow and sign.
// Returns LONG_MAX or LONG_MIN on overflow conditions.
long	ft_atol(const char *str)
{
    long	result;
    int		sign;

    result = 0; // Initialize result to zero.
    sign = 1;   // Default sign is positive.
    if (*str == '-' || *str == '+') // Check for sign.
    {
        if (*str == '-')
            sign = -1; // If negative, set sign accordingly.
        str++;
    }
    // Convert string to long, checking for overflows.
    while (*str >= '0' && *str <= '9')
    {
        if (result > (LONG_MAX - (*str - '0')) / 10) // Check for overflow before multiplying.
        {
            return (sign == 1 ? LONG_MAX : LONG_MIN); // Return appropriate limit based on sign.
        }
        result = result * 10 + (*str - '0'); // Update result.
        str++;
    }
    return (result * sign); // Return the final result with sign.
}

// Trims leading and trailing whitespace from a given string.
// Returns a new string without the extra whitespace, freeing the old one.
char	*trim_whitespace(char *str)
{
    int		start;
    int		end;

    start = 0;
    // Find the first non-whitespace character.
    while (str[start] && ft_isspace2(str[start]))
        start++;
    end = ft_strlen(str) - 1;
    // Find the last non-whitespace character.
    while (end > start && ft_isspace2(str[end]))
        end--;
    return (ft_substr(str, start, end - start + 1)); // Return the trimmed substring.
}

// Validates if the string is a valid number (signed or unsigned).
// Returns 1 if valid, 0 otherwise.
int	is_valid_number(char *str)
{
    if (!str || *str == '\0') // Empty or NULL string is invalid.
        return (0);
    if (*str == '-' || *str == '+') // Check for sign.
        str++;
    if (*str == '\0') // Single sign is not valid.
        return (0);
    while (*str) // Check all characters.
    {
        if (!ft_isdigit(*str)) // If any character is not a digit, return invalid.
            return (0);
        str++;
    }
    return (1); // All checks passed; return valid.
}

// Handles exit errors in the shell, printing an error message to STDERR.
// Sets the global exit status to 255 and exits.
void	exit_error(t_global *global, char *arg)
{
    ft_putstr_fd("exit: ", STDERR_FILENO); // Print error prefix.
    ft_putstr_fd(arg, STDERR_FILENO); // Print the invalid argument.
    ft_putstr_fd(": numeric argument required\n", STDERR_FILENO); // Print specific error message.
    global->exit_status = 255; // Set exit status to 255 for error.
    exit(255); // Exit the program.
}

// Implements the shell exit command.
// Takes global state and arguments from the user to determine exit status.
// Validates input, trims whitespace, and handles errors/exceptions.
void	ft_exit(t_global *global, char **args)
{
    char	*trimmed_arg; // Holds the trimmed argument.
    long	ex_status; // Variable for the parsed exit status.

    if (global->root) // Check if in the root shell context.
        ft_putstr_fd("exit\n", 1); // Print exit message.
    if (!args[1]) // If no arguments provided, exit with current status.
        exit(global->exit_status);
    trimmed_arg = trim_whitespace(args[1]); // Trim whitespace from the argument.
    if (!is_valid_number(trimmed_arg)) // Validate the trimmed argument.
        exit_error(global, args[1]); // Handle invalid numeric input.
    ex_status = ft_atol(trimmed_arg); // Convert valid trimmed string to long.
    free(trimmed_arg); // Free allocated memory for the trimmed argument.
    if (ex_status < 0 || ex_status > 255) // Normalize exit status to range 0-255.
        ex_status = ex_status % 256;
    if (args[2]) // Check for too many arguments.
    {
        ft_putstr_fd("exit: too many arguments\n", STDERR_FILENO); // Warn user of excess arguments.
        global->exit_status = 1; // Set exit status to 1 for error.
        return ; // Early return to remain in the shell.
    }
    global->exit_status = ex_status; // Update the global exit status for proper exit.
    exit(ex_status); // Exit the shell with the specified status.
}

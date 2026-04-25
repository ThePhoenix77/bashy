#include "bashy.h"

/**
 * Checks if the given string consists solely of '-n' flags,
 * indicating that output should not end with a newline.
 *
 * @param s The string to check.
 * @return 1 if the string is a valid '-n' flag, 0 otherwise.
 */
int	is_all_n(char *s)
{
    int	i;

    i = 1; // Start checking from the first character after '-'
    if (s[0] != '-') // The first character must be '-'
        return (0);
    // Check if all remaining characters are 'n'
    while (s[i])
    {
        if (s[i] != 'n')
            return (0);
        i++;
    }
    return (1); // All characters were 'n', return true.
}

/**
 * Implements the echo command, which outputs the provided arguments
 * to standard output. If arguments start with '-n', they are printed 
 * without a trailing newline.
 *
 * @param global Pointer to the global state, used for setting exit status.
 * @param args The arguments to be printed.
 */
void	ft_echo(t_global *global, char **args)
{
    int	i;
    int	nl_flag; // Flag to indicate whether to print a newline.
    int	args_num; // The total number of arguments.
    int	start_index; // Index to start printing arguments from.

    args_num = arg_size(args); // Get the number of arguments.
    nl_flag = 1; // Default to print a newline.
    i = 1; // Start checking from the first argument.

    // Detect whether all leading arguments are '-n' options.
    while (args[i] && is_all_n(args[i]))
    {
        nl_flag = 0; // Set flag to false if '-n' is detected.
        i++;
    }
    start_index = i; // Record where actual arguments start.
    i = start_index; // Reset index to start processing arguments.

    // Print all the arguments.
    while (args[i] != NULL)
    {
        ft_putstr_fd(args[i], STDOUT_FILENO); // Print current argument.
        // Print a space if not the last argument.
        if (i < start_index + args_num - 1 && args[i + 1] != NULL)
            ft_putstr_fd(" ", STDOUT_FILENO);
        i++;
    }
    // Print a newline if no '-n' options were found at the start.
    if (nl_flag)
        ft_putstr_fd("\n", STDOUT_FILENO);

    global->exit_status = 0; // Set the exit status to 0 to indicate success.
}

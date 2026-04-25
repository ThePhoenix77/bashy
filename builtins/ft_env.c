#include "bashy.h"

/**
 * Prints the current environment variables to standard output.
 * Each variable is printed on a new line, but only those 
 * that contain an '=' character are included.
 *
 * @param global Pointer to the global state, which contains the 
 *               linked list of environment variables (env_list).
 */
void	ft_env(t_global *global)
{
    t_env_list	*current; // Pointer to iterate through the environment list.

    current = global->env_list; // Start from the head of the environment list.
    while (current) // Iterate through the linked list.
    {
        // Check if the current node's content contains an '=' character,
        // indicating it is a valid environment variable.
        if (ft_strchr(current->content, '='))
        {
            // Print the environment variable followed by a newline.
            ft_putstr_fd(current->content, STDOUT_FILENO);
            ft_putstr_fd("\n", STDOUT_FILENO);
        }
        current = current->next; // Move to the next environment variable.
    }
}

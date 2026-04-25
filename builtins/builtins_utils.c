#include "bashy.h"

// Function to copy a string from src to dest.
// Ensures that dest is properly null-terminated after the copy.
char	*ft_strcpy(char *dest, const char *src)
{
    char	*dest_ptr = dest; // Pointer initialization for destination

    // Copy each character from src to dest until we hit the null terminator.
    while (*src)
    {
        *dest_ptr++ = *src++; // Copy character and increment both pointers.
    }
    *dest_ptr = '\0'; // Null-terminate the destination string.
    return (dest); // Return the pointer to the destination string.
}

// Function to export environment variables without any arguments.
// Iterates over the global environment list and prints each variable.
void	export_without_args(t_global *global)
{
    t_env_list	*tmp = global->env_list; // Start with the head of the env list.
    char		*equal_sign; // Pointer to the position of '=' in the variable.

    // Loop through the linked list of environment variables.
    while (tmp)
    {
        ft_putstr_fd("declare -x ", 1); // Print the "declare -x" prefix.
        equal_sign = ft_strchr(tmp->content, '='); // Look for the '=' character.
        if (equal_sign)
        {
            // If '=' is found, print the variable name and its value.
            write(1, tmp->content, equal_sign - tmp->content); // Print variable name.
            ft_putstr_fd("=\"", 1);
            ft_putstr_fd(equal_sign + 1, 1); // Print variable value.
            ft_putstr_fd("\"\n", 1);
        }
        else
        {
            // Print variable name only if '=' is not found (unset variable).
            ft_putstr_fd(tmp->content, 1);
            ft_putstr_fd("\n", 1);
        }
        tmp = tmp->next; // Move to the next environment variable.
    }
}

// Function to find a variable by its key in the environment list.
// Returns a pointer to the environment list node containing the variable or NULL.
t_env_list	*find_var(t_env_list *env_list, char *key)
{
    size_t	key_len = ft_strlen(key); // Get the length of the key.

    // Traverse the environment list until the desired variable is found.
    while (env_list)
    {
        // Compare the content of the current node with the key.
        if (ft_strncmp(env_list->content, key, key_len) == 0
            && (env_list->content[key_len] == '=' // Check for '=' follow-up.
                || env_list->content[key_len] == '\0')) // Handles no '=' case.
            return (env_list); // Return the found variable.
        env_list = env_list->next; // Move to the next node in the list.
    }
    return (NULL); // Return NULL if the variable is not found.
}

// Check if character is valid for variable names.
// Variable names can contain alphanumeric characters and underscores.
int	is_valid_name_char(char c)
{
    return (ft_isalnum(c) || c == '_'); // Return true for valid name characters.
}

// Validate the variable name according to shell rules.
// Returns 0 if valid, -1 if invalid.
int	name_checker(char *s)
{
    int	i = 0; // Initialize the index for string traversal.

    // Check if the string is NULL or empty.
    if (!s || s[0] == '\0')
        return (-1); // Invalid if NULL or empty.

    // Check if the first character is an alphabet letter or underscore.
    if (!ft_isalpha(s[0]) && s[0] != '_')
        return (-1); // Invalid if the first character is not valid.

    // Check all characters until an '=' or end of string.
    while (s[i] && s[i] != '=' && !(s[i] == '+' && s[i + 1] == '='))
    {
        if (!is_valid_name_char(s[i])) // Invalid if character fails validity check.
            return (-1);
        i++;
    }
    // Handle the specific case where '+' must be followed by '=' at start.
    if (s[i] == '+' && s[i + 1] == '=' && i == 0)
        return (-1); // Invalid if '+' appears at the start.
    return (0); // Return 0 for valid variable name.
}

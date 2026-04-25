#include "bashy.h"

/* This function counts the number of strings in the 'args' array,
   starting from index 1, and returns the count. It leverages a simple
   while loop that increments until a NULL pointer is encountered. */
int	arg_size(char **args)
{
    int	args_num;
    int	i;

    args_num = 0; // Initialize args_num to zero for counting
    i = 1; // Starts counting from the second argument
    while (args[i] != NULL) // Loop until we hit a NULL
    {
        args_num++; // Increment count for each valid argument
        i++; // Move to the next argument
    }
    return (args_num); // Return the total count
}

/* Allocates memory for a new string, duplicates up to 'n' characters
   from 'src', and ensures the resultant string is null-terminated. 
   A NULL check on malloc is done to prevent memory issues. */
char	*ft_strndup(char *src, int n)
{
    int		i;
    char	*dup;

    i = 0; // Index for copying characters
    dup = (char *)malloc(n + 1); // Allocate memory for duplication
    if (!dup) // Check for successful allocation
        return (NULL); // Return NULL if malloc fails
    while (src[i] && i < n) // Copy up to 'n' characters or until src end
    {
        dup[i] = src[i]; // Duplicate current character
        i++; // Move to the next character
    }
    dup[i] = '\0'; // Null-terminate the new string
    return (dup); // Return the duplicated string
}

/* Handles the case where a variable key already exists in the environment.
   If the variable is not found, it can potentially add a NULL update for it. */
void	existing_var_case(t_global *global, char *key)
{
    t_env_list	*existing_var;

    existing_var = find_var(global->env_list, key); // Search for key in the environment list
    if (!existing_var || existing_var->content == NULL) // If not found or content is NULL
        add_or_update_env_var(global, key, NULL, SHOW); // Update the environment variable, showing it to the user
    free(key); // Free the memory allocated for the key to prevent leaks
}

/* Processes a key-value pair from 'content'. It splits the string at the 
   '=' character, duplicates the key and value, and adds/updates the environment. */
void	handle_key_value(t_global *global, char *content)
{
    char	*equal_sign;
    char	*key;
    char	*value;
    size_t	key_length;

    equal_sign = ft_strchr(content, '='); // Find the position of the '='
    if (equal_sign) // If an '=' is found
    {
        key_length = equal_sign - content; // Calculate length of the key
        key = ft_strndup(content, key_length); // Duplicate the key
        value = ft_strdup(equal_sign + 1); // Duplicate everything after '=' as the value
        if (key && value) // Ensure duplicates were successful
        {
            add_or_update_env_var(global, key, value, HIDE); // Add/update the environment variable
            free(key); // Free the key memory
            free(value); // Free the value memory
        }
    }
    else // If there is no '='
    {
        key = ft_strdup(content); // Simply duplicate the content as key 
        if (key)
            existing_var_case(global, key); // Handle the case for existing variables
    }
}

/* Adds a new node to the environment list, handling the case where the 
   list may be empty and ensuring proper linking of nodes. */
void	add_export_node(t_env_list **head, t_env_list *new)
{
    t_env_list	*last;

    if (!head || !new) // Check for invalid inputs
        return; // Do nothing if head or new is NULL
    if (*head == NULL) // If the list is empty
        *head = new; // Set the head to the new node
    else
    {
        last = *head; // Start from the head
        while (last->next != NULL) // Traverse to the end of the list
            last = last->next; // Move to the next node
        last->next = new; // Link the last node to the new node
        new->prev = last; // Set the previous link for the new node
    }
}

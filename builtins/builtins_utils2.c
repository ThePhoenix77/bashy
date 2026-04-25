#include "bashy.h"

/**
 * Creates a new content string in the format "key=value".
 * The caller is responsible for freeing the returned string.
 * 
 * @param key The environment variable key.
 * @param value The environment variable value (can be NULL).
 * @return Pointer to the newly created string, or NULL on failure.
 */
char	*create_new_content(char *key, char *value)
{
    size_t	total_len;
    char	*new_content;

    // Calculate the total length needed for the new string,
    // accounting for '=' and null terminator.
    if (value)
        total_len = ft_strlen(key) + ft_strlen(value) + 2;
    else
        total_len = ft_strlen(key) + 1;

    // Allocate memory for the new content string.
    new_content = malloc(total_len);
    if (!new_content)
    {
        perror("malloc"); // Print error if allocation fails.
        return (NULL);
    }

    // Copy the key into the new content.
    ft_strcpy(new_content, key);
    if (value)
    {
        new_content[ft_strlen(key)] = '='; // Add '=' if value is present.
        ft_strcpy(new_content + ft_strlen(key) + 1, value); // Append the value.
    }
    return (new_content); // Return the constructed string.
}

/**
 * Checks if the given string starts with the specified prefix.
 *
 * @param str The string to check.
 * @param prest The prefix to match against.
 * @return 1 if str starts with prest, 0 otherwise.
 */
int	starts_with(char *str, char *prest)
{
    return (ft_strncmp(str, prest, ft_strlen(prest)) == 0);
}

/**
 * Updates an existing environment variable's content with new data.
 *
 * @param env_var Pointer to the environment variable to update.
 * @param new_content The new content to replace the old one.
 */
void	update_env_var(t_env_list *env_var, char *new_content)
{
    free(env_var->content); // Free old content to prevent memory leak.
    env_var->content = new_content; // Point to the new content.
}

/**
 * Creates a new node for the environment variable linked list.
 * Frees the new content if allocation fails.
 *
 * @param new_content The content to associate with the new node.
 * @param type The type of the environment variable.
 * @return Pointer to the newly created node, or NULL on failure.
 */
t_env_list	*create_new_node(char *new_content, int type)
{
    t_env_list	*new_node;

    new_node = malloc(sizeof(t_env_list)); // Allocate memory for the new node.
    if (!new_node)
    {
        perror("malloc"); // Print error if allocation fails.
        free(new_content); // Clean up new_content to avoid memory leak.
        return (NULL);
    }
    new_node->content = new_content; // Set the node's content.
    new_node->next = NULL; // Initialize next pointer to NULL.
    new_node->prev = NULL; // Initialize prev pointer to NULL.
    new_node->type = type; // Set the type of the environment variable.
    return (new_node); // Return the new node.
}

/**
 * Adds or updates an environment variable in the global environment list.
 * If the variable exists, update its value. If not, create a new node.
 *
 * @param global Pointer to the global state containing the environment list.
 * @param key The key of the environment variable to add or update.
 * @param value The value to assign to the environment variable.
 * @param type The type of the environment variable.
 */
void	add_or_update_env_var(t_global *global, char *key,
        char *value, int type)
{
    t_env_list	*env_var;
    char		*new_content;
    t_env_list	*new_node;

    // Check if the environment variable already exists.
    env_var = find_var(global->env_list, key);
    
    // Create new content string "key=value".
    new_content = create_new_content(key, value);
    if (!new_content) // Ensure allocation was successful.
        return ;

    if (env_var) // If the variable exists, update it.
    {
        update_env_var(env_var, new_content);
        env_var->type = type; // Update the type if necessary.
    }
    else // If the variable does not exist, create a new node.
    {
        new_node = create_new_node(new_content, type);
        if (new_node) // Only add if node creation was successful.
            add_export_node(&global->env_list, new_node);
    }
}

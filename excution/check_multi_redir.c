#include "bashy.h"

// Create a new redirection node by copying an existing one
t_redir	*new_coipe_list(t_redir *node)
{
	t_redir	*new;

	new = (t_redir *)malloc(sizeof(t_redir)); // Allocate memory for new redirection node
	if (!new)
		return (NULL); // Return NULL on failure
	new->file_name = ft_strdup(node->file_name); // Duplicate the file name
	if (!new->file_name)
	{
		free(new); // Free allocated memory if strdup fails
		return (NULL);
	}
	new->file_type = node->file_type; // Copy file type
	new->type = node->type; // Copy type
	new->next = NULL; // Initialize next pointer to NULL
	return (new);
}

// Open files based on the redirection type and handle errors
bool	open_redir_files(t_redir *node)
{
	int	fd;

	if (node->type == REDIR_OUT)
	{
		fd = open(node->file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644); // Open file for output
		if (fd < 0)
			return (perror("bashy: open"), FAILURE); // Handle open error
	}
	else if (node->type == DOUBLE_QUOTE)
	{
		fd = open(node->file_name, O_WRONLY | O_CREAT | O_APPEND, 0644); // Open file for appending
		if (fd < 0)
			return (perror("bashy: open"), FAILURE);
	}
	else if (node->type == REDIR_IN)
	{
		fd = open(node->file_name, O_RDONLY, 0644); // Open file for input
		if (fd < 0)
			return (perror("bashy: open"), FAILURE);
	}
	if (node->file_type == HERE_DOC_FILE)
		return (unlink(node->file_name), SUCCESS); // Unlink here-document files
	else if (node->type == ERROR_DIS)
		return (SUCCESS); // Handle error-disallowing types
	return (close(fd), SUCCESS); // Close file descriptor and return success
}

// Add a copied redirection node to the global state
void	add_copie_list(t_global *global, t_redir **new_head, t_redir *old)
{
	t_redir	*new;

	new = new_coipe_list(old); // Create a new copied node
	if (!new)
		(free_redir_list(new_head), malloc_failed(global)); // Handle malloc failure
	add_back_redir(new_head, new); // Add new node to the linked list
}

// Check for double redirection and manage them accordingly
bool	check_doubel_redir(t_global *global, t_redir **redir_list)
{
	t_redir	*tmp;
	t_redir	*new_head;

	new_head = NULL; // Initialize new head for the new redirection list
	tmp = *redir_list; // Start from the original redirection list
	while (tmp)
	{
		while (tmp->next && (tmp->type == tmp->next->type) // Check for consecutive redirections
			&& (tmp->file_type != ERROR_DIS))
		{
			if (open_redir_files(tmp) == FAILURE) // Open the file for the current node
				return (free_redir_list(&new_head), FAILURE); // Handle failure
			tmp = tmp->next; // Move to the next node
		}
		add_copie_list(global, &new_head, tmp); // Add the current node to the new list
		if (tmp->file_type == ERROR_DIS) // Break on error-disallowing types
			break ;
		tmp = tmp->next; // Move to the next node
	}
	free_redir_list(redir_list); // Free the original list
	*redir_list = new_head; // Update the original pointer to the new list
	return (SUCCESS); // Return success
}

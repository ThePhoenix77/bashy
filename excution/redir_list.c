#include "bashy.h"

/**
 * new_vertex - Creates a new redirection node based on the current token.
 * @global: Pointer to the global state structure.
 * @node: Pointer to the current token in the token list.
 * 
 * This function allocates memory for a new redirection node, copies the 
 * redirection file name from the token list, and assigns the redirection type 
 * (input, output, or append). If memory allocation fails, it calls 
 * `malloc_failed` to handle the error.
 * 
 * Return: A pointer to the newly created redirection node.
 */
t_redir	*new_vertex(t_global *global, t_lst *node)
{
	t_redir	*new;

	// Allocate memory for the new redirection node
	new = (t_redir *)malloc(sizeof(t_redir));
	if (!new)
		malloc_failed(global);  // Handle memory allocation failure

	// Copy the redirection file name from the next token in the list
	new->file_name = ft_strdup(node->next->content);
	if (!new->file_name)
	{
		free(new);  // Free the redirection node on failure
		malloc_failed(global);  // Handle memory allocation failure
	}

	// Assign redirection type and file type
	new->type = node->type;         // Redirection type (>, >>, <)
	new->file_type = node->next->type;  // File type (regular or heredoc)
	new->next = NULL;               // Initialize the next pointer to NULL

	return (new);
}

/**
 * add_back_redir - Adds a new redirection node to the end of the redirection list.
 * @head: Double pointer to the head of the redirection list.
 * @new: Pointer to the new redirection node to add.
 * 
 * This function appends the new redirection node to the end of the existing 
 * redirection list. If the list is empty, the new node becomes the head of the list.
 */
void	add_back_redir(t_redir **head, t_redir *new)
{
	t_redir	*last;

	last = *head;

	// If both the head and new node are NULL, there's nothing to add
	if (!(*head) && !new)
		return ;

	// If the list is empty, the new node becomes the head
	if (!(*head))
		*head = new;
	else
	{
		// Traverse the list to find the last node
		while (last->next)
			last = last->next;

		// Add the new node to the end of the list
		last->next = new;
	}
}

/**
 * build_redir_list - Builds a redirection list from the token list for a command.
 * @global: Pointer to the global state structure.
 * @node: Double pointer to the current token in the token list.
 * 
 * This function processes a redirection token, creates a new redirection node,
 * and appends it to the command's redirection list. It also moves the token
 * pointer forward to handle the file name following the redirection operator.
 */
void	build_redir_list(t_global *global, t_lst **node)
{
	t_redir		*new;
	t_exc_list	*last;

	// Find the last command node in the execution list
	last = last_exc_node(global->root);

	// Allocate memory for the new redirection node
	new = (t_redir *)malloc(sizeof(t_redir));
	if (!new)
		malloc_failed(global);  // Handle memory allocation failure

	new->next = NULL;   // Initialize the next pointer to NULL
	new->type = (*node)->type;  // Set the redirection type (>, >>, <)

	// Add the new redirection node to the command's redirection list
	add_back_redir(&last->redir, new);

	// Move to the next token, which should be the file name
	(*node) = (*node)->next;

	// Copy the file name from the token list to the redirection node
	new->file_name = ft_strdup((*node)->content);
	if (!new->file_name)
		malloc_failed(global);  // Handle memory allocation failure

	// Move to the next token (after the file name)
	(*node) = (*node)->next;
}

#include "bashy.h"

// Create a new command argument node
t_cmd_args	*new_cmd_element(t_global *global, char *content)
{
	t_cmd_args	*new;

	new = (t_cmd_args *)malloc(sizeof(t_cmd_args)); // Allocate memory for new command argument
	if (!new)
		malloc_failed(global); // Handle memory allocation failure
	new->next = NULL; // Initialize the next pointer to NULL
	new->content = ft_strdup(content); // Duplicate the content string
	if (!new->content)
	{
		free(new); // Free allocated memory if strdup fails
		malloc_failed(global); // Handle memory allocation failure
	}
	return (new); // Return the newly created command argument
}

// Add a new command argument node to the end of the list
void	add_back_element(t_cmd_args **head, t_cmd_args *new)
{
	t_cmd_args	*last;

	if (!(*head) && !new) // If both head and new are NULL, do nothing
		return ;
	if (!(*head)) // If the list is empty, set the head to new
		*head = new;
	else
	{
		last = *head; // Start from the head
		while (last->next) // Traverse to the end of the list
			last = last->next;
		last->next = new; // Link the new node at the end
	}
}

// Store command arguments in the last execution node
void	store_cmd_args(t_global *global, t_lst **node)
{
	t_cmd_args	*new;
	t_exc_list	*last;

	last = last_exc_node(global->root); // Get the last execution node
	while (*node && (*node)->type == WORD) // While there are nodes of type WORD
	{
		new = (t_cmd_args *)malloc(sizeof(t_cmd_args)); // Allocate memory for new command argument
		if (!new)
			malloc_failed(global); // Handle memory allocation failure
		new->next = NULL; // Initialize the next pointer to NULL
		add_back_element(&last->cmd_args, new); // Add the new argument to the last execution node
		new->content = ft_strdup((*node)->content); // Duplicate the content of the current node
		if (!new->content)
			malloc_failed(global); // Handle memory allocation failure
		*node = (*node)->next; // Move to the next node
	}
}

// Add a command to the list of command arguments in an execution node
void	add_list_cmd(t_global *global, t_exc_list *exc, t_lst **node)
{
	t_cmd_args	*new_cmd_node;

	new_cmd_node = new_cmd_element(global, (*node)->content); // Create a new command argument
	add_back_element(&exc->cmd_args, new_cmd_node); // Add it to the execution node's command arguments
	(*node) = (*node)->next; // Move to the next node
}

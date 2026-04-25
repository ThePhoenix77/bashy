#include "bashy.h"

// Create a new execution list node
t_exc_list	*new_exc_spot(t_global *global, t_type_node type)
{
	t_exc_list	*new;

	new = (t_exc_list *)malloc(sizeof(t_exc_list)); // Allocate memory for a new execution node
	if (!new)
		malloc_failed(global); // Handle memory allocation failure
	new->type = type; // Set the type of the execution node
	new->redir = NULL; // Initialize redirection to NULL
	new->next = NULL; // Initialize the next pointer to NULL
	new->fd.in = -1; // Initialize input file descriptor
	new->fd.out = -1; // Initialize output file descriptor
	new->cmd_args = NULL; // Initialize command arguments to NULL
	return (new); // Return the newly created execution node
}

// Get the last node in the execution list
t_exc_list	*last_exc_node(t_exc_list *head)
{
	if (!head) // If the list is empty, return NULL
		return (NULL);
	while (head->next) // Traverse to the last node
		head = head->next;
	return (head); // Return the last node
}

// Add a new execution node to the end of the list
void	add_back_exc(t_exc_list **head, t_exc_list *new)
{
	t_exc_list	*last_node;

	if (!new && !(*head)) // If both new and head are NULL, do nothing
		return ;
	if (!(*head)) // If the list is empty, set head to new
		*head = new;
	else
	{
		last_node = last_exc_node((*head)); // Find the last node in the list
		last_node->next = new; // Link the new node at the end
	}
}

// Count the number of nodes in the execution list
int	exc_list_size(t_exc_list *exc)
{
	int	i;

	i = 0; // Initialize the counter to 0
	while (exc) // Traverse the list
	{
		i++; // Increment the counter for each node
		exc = exc->next; // Move to the next node
	}
	return (i); // Return the total count of nodes
}

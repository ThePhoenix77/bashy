#include "bashy.h"

// Function to create a new node for the linked list.
t_lst	*new_node(t_global *global)
{
	t_lst	*new;

	new = (t_lst *)malloc(sizeof(t_lst)); // Allocate memory for a new node.
	if (new == NULL)
		malloc_failed(global); // Handle memory allocation failure.
	new->next = NULL; // Initialize pointers.
	new->prev = NULL;
	new->content = NULL;
	new->type = WORD; // Set default type.
	new->state = GENERAL; // Set default state.
	new->len = 0; // Initialize length.
	return (new); // Return the newly created node.
}

// Function to find the last node in the linked list.
t_lst	*last_node(t_lst *head)
{
	if (!head)
		return (NULL); // Return NULL if the list is empty.
	while (head->next) // Traverse to the last node.
		head = head->next;
	return (head); // Return the last node.
}

// Function to add a node to the end of the linked list.
bool	add_back(t_lst **head, t_lst **new)
{
	t_lst	*last;

	if (*new == NULL) // Check if the new node is NULL.
		return (FAILURE);
	if (!(*head)) // If the list is empty, set the new node as the head.
	{
		*head = *new;
		(*new)->next = NULL; // Ensure the new node points to NULL.
		(*new)->prev = NULL; // Ensure the new node has no previous node.
	}
	else
	{
		last = last_node(*head); // Find the last node.
		(*new)->prev = last; // Link the new node back to the last node.
		last->next = *new; // Link the last node to the new node.
	}
	return (SUCCESS); // Return success after adding the node.
}

// Function to free the entire linked list.
void	free_lst(t_lst	**head)
{
	t_lst	*current;
	t_lst	*next_node;

	if (head == NULL || *head == NULL) // Check if the list is empty.
		return ;
	current = *head; // Start from the head.
	while (current != NULL) // Traverse the list.
	{
		next_node = current->next; // Store the next node.
		if (current->content)
			free(current->content); // Free the content if it exists.
		free(current); // Free the current node.
		current = next_node; // Move to the next node.
	}
	*head = NULL; // Set the head to NULL after freeing.
}

// Function to add a new node to the global linked list based on the current state.
void	add_list(t_global *global)
{
	t_lst	*new;

	new = new_node(global); // Create a new node.
	new->content = global->content; // Set the content from global state.
	new->state = global->state; // Set the state from global state.
	new->type = global->type; // Set the type from global state.
	new->len = ft_strlen(global->content); // Set the length of the content.
	add_back(&global->head, &new); // Add the new node to the linked list.
	global->list_len++; // Increment the length of the list.
	global->content = NULL; // Reset the global content.
	global->type = WORD; // Reset the type to default.
	global->state = GENERAL; // Reset the state to default.
}

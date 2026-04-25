#include "bashy.h"

// Function to free allocated memory associated with the global state.
void	free_memory(t_global *global)
{
	// Check and free the linked list of commands if it exists
	if (global->head != NULL)
		free_lst(&global->head);
	
	// Free the line input buffer if allocated
	if (global->line_input != NULL)
		free(global->line_input);
	
	// Free the list of expanded variables if it exists
	if (global->exp_head != NULL)
		free_exp_list(&global->exp_head);
	
	// Free the previous command list if it exists
	if (global->pre_head != NULL)
		free_lst(&global->pre_head);
	
	// Free the 2D array of environment variables if allocated
	if (global->myenv)
		free_2d_array(&global->myenv);
	
	// Duplicate free check on previous command list (remove if redundant)
	if (global->pre_head)
		free_lst(&global->pre_head);
	
	// Free the root execution list if it exists
	if (global->root)
		free_exc_list(&global->root);
	
	// Free the root tree structure if it exists
	if (global->root_tree)
		free_tree(&global->root_tree);
}

// Function to free a 2D array of strings.
void	free_2d_array(char ***leaks)
{
	int	i;

	// Ensure there are elements to free
	if (leaks[0] == NULL || leaks[0][0] == NULL)
		return ;
	
	// Free each string in the array
	i = 0;
	while ((*leaks)[i])
		free((*leaks)[i++]);
	
	// Free the array itself and set pointer to NULL to avoid dangling references
	free(*leaks);
	*leaks = NULL;
}

// Function to free a linked list of command arguments.
void	free_cmd_list(t_cmd_args **head)
{
	t_cmd_args	*tmp;

	// Iterate through the list and free each command node
	while (*head)
	{
		tmp = *head; // Store the current head
		(*head) = (*head)->next; // Move to the next node
		if (tmp->content) // Free the command content if allocated
			free(tmp->content);
		free(tmp); // Free the command node itself
	}
	*head = NULL; // Set head to NULL to avoid dangling pointer
}

// Function to free a linked list of redirection structures.
void	free_redir_list(t_redir **head)
{
	t_redir	*tmp;

	// If the list is empty, there's nothing to free
	if ((*head) == NULL)
		return ;
	
	// Iterate through the list and free each redirection node
	while (*head)
	{
		tmp = *head; // Store the current head
		*head = (*head)->next; // Move to the next node
		if (tmp->file_name) // Free the file name if allocated
			free(tmp->file_name);
		free(tmp); // Free the redirection node itself
	}
	*head = NULL; // Set head to NULL to avoid dangling pointer
}

// Function to free a linked list of execution contexts.
void	free_exc_list(t_exc_list **head)
{
	t_exc_list	*tmp;

	// Iterate through the execution list
	while (*head)
	{
		tmp = (*head); // Store the current head
		*head = (*head)->next; // Move to the next node
		// Free associated command and redirection lists
		free_cmd_list(&tmp->cmd_args);
		free_redir_list(&tmp->redir);
		free(tmp); // Free the execution node itself
	}
}

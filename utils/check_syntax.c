#include "bashy.h"

// Function to count the number of HERE_DOC nodes in the linked list.
// Returns FAILURE if the count exceeds 16, otherwise SUCCESS.
bool	check_num_heredoc(t_global *global)
{
	int		i;
	t_lst	*tmp;

	i = 0;
	tmp = global->head;

	// Traverse the linked list to count HERE_DOC occurrences
	while (tmp)
	{
		if (tmp->type == HERE_DOC)
			i++;
		tmp = tmp->next;
	}

	// Limit the number of HERE_DOC to 16
	if (i > 16)
		return (FAILURE);
	return (SUCCESS);
}

// Function to print error messages related to HERE_DOC syntax.
// It checks the previous nodes to provide context in case of a syntax error.
void	ft_print_error(t_global *global, t_lst **node)
{
	// Check if the previous node(s) are of type HERE_DOC
	if ((((*node)) && ((*node)->prev && (*node)->prev->type == HERE_DOC))
		|| (((*node)->prev) && (*node)->prev->prev
			&& (*node)->prev->prev->type == HERE_DOC))
	{
		// Print specific error message and invoke HERE_DOC checks
		ft_putstr_fd(ERROR, 2);
		ft_putchar_fd((*node)->content[0], 2);
		ft_putstr_fd("'\n", 2);
		check_here_doc(global);
		return ;
	}

	// Generic error handling if no specific context is found
	check_here_doc(global);
	ft_putstr_fd(ERROR, 2);
	ft_putchar_fd((*node)->content[0], 2);
	ft_putstr_fd("'\n", 2);
}

// Function to check the overall syntax of the command list.
// Implements a finite state machine (FSM) using a transition table to validate syntax.
bool	check_syntax(t_global *global)
{
	t_lst		*tmp;
	int			v;
	// Transition table to handle different syntax states
	static int	tr[6][7] = {
		{2, 3, 3, 3, 3, 0, 0},
		{2, 3, 3, 3, 3, 4, 5}, 
		{2, 0, 0, 0, 0, 0, 6},
		{2, 3, 3, 3, 3, 0, 6}, 
		{2, 3, 3, 3, 3, 4, 0},
		{2, 3, 3, 3, 3, 0, 0}
	};

	tmp = global->head;
	v = 1; // Initial state
	// Traverse the linked list while checking syntax using FSM
	while (tmp)
	{
		v = tr[v - 1][tmp->type - 1]; // Update state based on current node type
		if (!v) // Invalid state detected
		{
			ft_print_error(global, &tmp); // Print error and return failure
			return (FAILURE);
		}
		tmp = tmp->next; // Move to the next node
	}
	// If the final state is valid (2), return success
	if (v == 2)
		return (SUCCESS);
	
	// Handle error for the last node in case of syntax failure
	tmp = last_node(global->head);
	ft_print_error(global, &tmp);
	return (FAILURE);
}

// Function to open a HEREDOC and read input until the end-of-file (EOF) string is encountered.
void	open_heredoc_sy(char *eof)
{
	char	*read;
	// Infinite loop to read user input until EOF is matched
	while (1)
	{
		read = readline("> "); // Prompt user for input
		if (!read || ft_strcmp(read, eof) == SUCCESS) // Exit if EOF matched or read failed
			break ;
		free(read); // Free the read input if not EOF
	}
	// Free the last read input if it was successful
	if (read)
		free(read);
}

// Function to check for HERE_DOCs in the command list and handle them accordingly.
void	check_here_doc(t_global *global)
{
	t_lst	*tmp;

	tmp = global->head;
	// Traverse the list to identify HERE_DOC nodes
	while (tmp)
	{
		if (tmp->type == HERE_DOC)
		{
			// If a HERE_DOC is found, check the next node for a WORD type
			if (tmp->next)
			{
				if (tmp->next->next && tmp->next->next->type == WORD)
					open_heredoc_sy(tmp->next->next->content); // Open HEREDOC with the EOF
			}
		}
		tmp = tmp->next; // Move to the next node
	}
}

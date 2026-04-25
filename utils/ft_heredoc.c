#include "bashy.h"

// Function to buffer lines from the heredoc into global content.
void	buff_heredoc(t_global *global, char *line)
{
	char	*tmp;
	char	*save;

	// Join the new line with a newline character for heredoc formatting.
	tmp = ft_strjoin2(line, "\n");
	if (!tmp) // Check for memory allocation failure.
	{
		free(line);
		malloc_failed(global);
	}
	free(line); // Free the input line after copying.
	// Concatenate current global content with the new line.
	save = ft_strjoin2(global->content, tmp);
	if (global->content)
		free(global->content); // Free previous global content.
	free(tmp); // Free temporary line storage.
	global->content = save; // Update global content with new data.
}

// Function to open and read lines for heredoc input until a delimiter is reached.
bool	open_heredoc(t_global *global, t_lst *lim_node)
{
	char	*line;

	global->content = NULL; // Initialize global content for heredoc.
	signal(SIGINT, handler); // Set signal handler for Ctrl+C interruption.
	while (1)
	{
		line = readline("> "); // Read a line from input.
		// Check if we can access the terminal.
		if (!ttyname(0))
		{
			open(ttyname(2), O_RDWR); // Open the terminal in read-write mode.
			return (free(line), FAILURE); // Free line and return failure.
		}
		// Check for end of heredoc or termination conditions.
		if (!line || ft_strcmp(line, lim_node->content) == SUCCESS)
		{
			free(lim_node->content); // Free the limit node content.
			lim_node->content = global->content; // Set limit node content to gathered lines.
			if (!line) // If line is NULL, indicate empty heredoc.
				empty_here_doc(global, lim_node);
			free(line); // Free the input line.
			break ; // Exit loop.
		}
		buff_heredoc(global, line); // Buffer the line into global content.
	}
	sig_hnd(); // Handle any signals after reading is complete.
	return (SUCCESS); // Return success after reading completes.
}

// Function to gather all WORD type nodes into the global content until a limit is reached.
void	get_limit_hrdc(t_global *global, t_lst **node)
{
	char	*save;
	t_lst	*hd_node;

	hd_node = *node; // Get the current node from the pointer.
	global->content = NULL; // Initialize global content.
	save = NULL; // Temporary storage for concatenation.
	while (hd_node && hd_node->type == WORD) // Continue while WORD type nodes are present.
	{
		// Concatenate current global content with the node's content.
		save = ft_strjoin2(global->content, hd_node->content);
		if (!save) // Check for memory allocation failure.
		{
			if (global->content)
				free(global->content);
			malloc_failed(global);
		}
		if (global->content)
			free(global->content); // Free previous global content.
		global->content = save; // Update global content with the new string.
		hd_node->type = SKIP; // Mark node as skipped to avoid reprocessing.
		hd_node = hd_node->next; // Move to the next node.
	}
}

// Function to check for heredoc failures and attempt to open the heredoc.
bool	here_doc_failed(t_global *global, t_lst *node)
{
	if (open_heredoc(global, node) == FAILURE) // Attempt to open heredoc and check for failures.
	{
		if (global->content)
			free(global->content); // Free any accumulated content on failure.
		return (FAILURE); // Return failure if heredoc opening fails.
	}
	return (SUCCESS); // Return success if opening succeeds.
}

// Main function to check for HERE_DOC nodes and process them accordingly.
bool	check_heredoc(t_global *global)
{
	t_lst	*tmp;

	tmp = global->head; // Start at the head of the list.
	while (tmp) // Traverse the linked list.
	{
		if (tmp->type == HERE_DOC) // Check if current node is a HERE_DOC type.
		{
			if (tmp->next->type == WORD) // If the next node is WORD, move to it.
				tmp = tmp->next;
			else // Otherwise, skip to the next next node.
				tmp = tmp->next->next;

			get_limit_hrdc(global, &tmp); // Gather limit lines into global content.

			if (tmp->content) // If we have content for this node.
			{
				free(tmp->content); // Free previous content.
				tmp->content = global->content; // Set current node content to gathered content.
				global->content = NULL; // Reset global content for future use.
			}

			if (here_doc_failed(global, tmp) == FAILURE) // Attempt to open heredoc; check for failure.
				return (FAILURE); // Return failure if heredoc processing fails.
			tmp->type = EXP_HERE_DOC; // Mark node as processed.
		}
		tmp = tmp->next; // Move to the next node.
	}
	return (SUCCESS); // Return success if all HERE_DOC nodes are processed.
}

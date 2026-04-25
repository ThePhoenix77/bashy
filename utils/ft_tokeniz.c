#include "bashy.h"

// Function to handle the extraction of operators from the input line.
// This function checks for whitespace and processes operators accordingly.
void	get_opr(t_global *global)
{
	// Skip any leading whitespace if in the GENERAL state.
	if ((global->line_input[0] == ' ' || global->line_input[0] == '\t' || global->line_input[0] == '\n')
		&& global->state == GENERAL)
		skip_ec_t(global, 1);

	// If the current character is not an operator, exit early.
	if (check_operator(global) == SUCCESS)
		return ;

	// Allocate memory for a single character operator; check for allocation failure.
	global->content = (char *)malloc(sizeof(char) * 2);
	if (global->content == NULL)
		malloc_failed(global);

	// Copy the operator into global content and update its type.
	ft_strlcpy(global->content, global->line_input, 2);
	global->type = global->content[0];
	check_type(global); // Validate the type of the operator.
	update_line(&global->line_input, 1, global); // Update the input line to move past the operator.
	add_list(global); // Add the identified operator to the linked list.
}

// Function to check if the current character is a valid operator.
// Returns SUCCESS or FAILURE based on the evaluation.
bool	check_operator(t_global *global)
{
	// If the character is not in the set of META_CHARS, it is not an operator.
	if (!ft_strchr(META_CHARS, global->line_input[0]))
		return (SUCCESS);

	// Handle quoted strings separately.
	if (global->line_input[0] == '\"' || global->line_input[0] == '\'')
	{
		get_str_inside_quote(global); // Process quoted string.
		global->state = GENERAL; // Reset state to GENERAL after processing quotes.
		global->type = WORD; // Set type to WORD.
		return (SUCCESS);
	}

	// Check for redirection operators and handle heredoc or redirection accordingly.
	if (global->line_input[0] == '>' || global->line_input[0] == '<')
	{
		if (check_heredoc_dred(global) == SUCCESS)
			return (SUCCESS);
	}

	// If input is empty or NULL, return SUCCESS to indicate end of processing.
	if (global->line_input[0] == '\0' || !global->line_input)
		return (SUCCESS);

	return (FAILURE); // If none of the conditions are met, return FAILURE.
}

// Function to delete trailing whitespace from the command list.
void	del_sp(t_global *global)
{
	t_lst	*last;

	last = last_node(global->head); // Get the last node in the linked list.
	// If the last node is of type WHITE_SPACE, free its resources.
	if (last->type == WHITE_SPACE)
	{
		free(last->content); // Free the content of the whitespace node.
		last->prev->next = NULL; // Update the previous node's next pointer.
		free(last); // Free the whitespace node itself.
	}
}

// Main tokenizer function to process the input line and tokenize it into commands and operators.
void	ft_tokeniz(t_global *global)
{
	int		len_un_sep; // Length of un-separated tokens.

	// Continue processing while there is input to tokenize.
	while (global->line_input && global->line_input[0] != '\0')
	{
		len_un_sep = ft_strlen_un_del(META_CHARS, global->line_input); // Calculate the length of un-delimited tokens.

		// If there are un-separated tokens present.
		if (len_un_sep)
		{
			// If the first character is not a meta character, get the word.
			if (!ft_strchr(META_CHARS, global->line_input[0]))
			{
				global->content = get_word(&global->line_input, len_un_sep, global); // Extract the word.
				add_list(global); // Add the word to the linked list.
			}
			else // If it's a meta character, handle as an operator.
				get_opr(global);
		}

		// If no un-separated tokens are found, treat the remaining input as a single token.
		if (len_un_sep == 0)
		{
			global->content = global->line_input; // Set content to the current line input.
			global->line_input = NULL; // Clear the line input to indicate end of processing.
			add_list(global); // Add the remaining input as a token to the list.
		}
	}

	del_sp(global); // Clean up any trailing whitespace at the end of processing.
	return ;
}

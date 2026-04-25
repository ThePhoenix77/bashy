#include "bashy.h"

/**
 * check_heredoc_dred - Checks if the current input contains a '<<' (heredoc) 
 *                      or '>>' (append) token.
 * @global: Pointer to the global state structure.
 * 
 * This function checks if the first two characters of the input are either
 * '<<' (indicating a heredoc) or '>>' (indicating append redirection). If found, 
 * it updates the global content and token type, advances the input pointer, 
 * and adds the token to the token list.
 * 
 * Return: SUCCESS if a token is found and processed, FAILURE otherwise.
 */
bool	check_heredoc_dred(t_global *global)
{
	// Check for '>>' (append redirection)
	if (global->line_input[0] == '>' && global->line_input[1] == '>')
	{
		global->content = ft_strdup(">>");  // Duplicate ">>" as token content
		global->type = DREDIR_OUT;          // Set token type to double output redirection
	}
	// Check for '<<' (heredoc)
	else if (global->line_input[0] == '<' && global->line_input[1] == '<')
	{
		global->content = ft_strdup("<<");  // Duplicate "<<" as token content
		global->type = HERE_DOC;            // Set token type to heredoc
	}

	// If a heredoc or double redirection is found, process the token
	if (global->type == HERE_DOC || global->type == DREDIR_OUT)
	{
		// Skip past the '<<' or '>>' in the input line
		update_line(&global->line_input, 2, global);

		// Add the new token to the token list
		add_list(global);

		return (SUCCESS); // Indicate that a token was successfully processed
	}

	// Return FAILURE if neither '<<' nor '>>' were found
	return (FAILURE);
}

/**
 * get_str_inside_quote - Extracts the string inside single or double quotes.
 * @global: Pointer to the global state structure.
 * 
 * This function checks if the current character is a quote (' or "). If so,
 * it switches the parsing state (e.g., to IN_DQUOTE or IN_SQUOTE), and extracts
 * the string inside the quotes. It handles the end of the quote and updates the
 * input line, adding the extracted string as a token to the token list.
 * 
 * If an unclosed quote is detected, it handles memory cleanup and signals an error.
 */
void	get_str_inside_quote(t_global *global)
{
	int	len_unt_qoute; // Length of the string up to the closing quote

	len_unt_qoute = 0;

	// Check if the current input character is a quote (either ' or ")
	if (global->line_input[0] == '\"' || global->line_input[0] == '\'')
	{
		// Update the parsing state based on the type of quote
		check_state(global);

		// Move the input pointer past the opening quote
		update_line(&global->line_input, 1, global);

		// Determine the length of the string up to the closing quote
		if (global->state == IN_DQUOTE)
			len_unt_qoute = ft_strlen_un_char(global->line_input, '\"'); // Find closing double quote
		if (global->state == IN_SQUOTE)
			len_unt_qoute = ft_strlen_un_char(global->line_input, '\''); // Find closing single quote

		// Handle unclosed quotes (len_unt_qoute < 0 indicates no closing quote found)
		if (len_unt_qoute < 0)
		{
			free_memory(global);                  // Clean up any allocated memory
			ft_putstr_fd(UNCLOSED_QUOTE, 2);      // Print error message to stderr
			global->exit_status = 258;            // Set exit status to indicate an error
			get_line(global);                     // Re-fetch the input (shell-specific behavior)
		}

		// Allocate memory for the content inside the quotes
		global->content = malloc(sizeof(char) * (len_unt_qoute + 1));
		if (global->content == NULL)
			return ((void) NULL); // Return if allocation fails (error handling elsewhere)

		// Copy the string inside the quotes into global->content
		ft_strlcpy(global->content, global->line_input, len_unt_qoute + 1);

		// Move the input pointer past the closing quote
		update_line(&global->line_input, len_unt_qoute + 1, global);

		// Add the extracted string as a token to the token list
		add_list(global);
	}
}

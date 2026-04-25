#include "bashy.h"

/**
 * get_word - Extracts a word from the input line.
 * @line: Double pointer to the current position in the input line.
 * @len: Length of the word to extract.
 * @global: Pointer to the global state structure.
 * 
 * Allocates memory for the extracted word, copies it from the input line,
 * and updates the line pointer to move past the extracted word.
 * 
 * Return: Pointer to the extracted word. If allocation fails, calls malloc_failed().
 */
char	*get_word(char **line, int len, t_global *global)
{
	char	*word;

	// Allocate memory for the word, including space for the null terminator
	word = malloc(sizeof(char) * (len + 1));
	if (!word)
		malloc_failed(global); // Handle allocation failure

	// Copy the word from the input line and ensure null-termination
	ft_strlcpy(word, *line, len + 1);

	// Update the input line to skip past the extracted word
	update_line(line, len, global);

	return (word);
}

/**
 * update_line - Updates the input line by removing the first 'len_skip' characters.
 * @line: Double pointer to the current position in the input line.
 * @len_skip: Number of characters to skip.
 * @global: Pointer to the global state structure.
 * 
 * This function creates a new string starting from 'line + len_skip', 
 * frees the old line, and replaces it with the updated string.
 */
void	update_line(char **line, int len_skip, t_global *global)
{
	char	*save;

	// Duplicate the remaining portion of the input line after 'len_skip' characters
	save = ft_strdup(*line + len_skip);
	if (!save)
		malloc_failed(global); // Handle allocation failure

	// Free the old line and replace it with the updated one
	free(*line);
	*line = save;
	save = NULL; // Clear temporary pointer
}

/**
 * skip_ec_t - Skips whitespace (spaces and tabs) in the input and optionally
 *             adds a whitespace token to the token list.
 * @global: Pointer to the global state structure.
 * @flag: Indicates whether to add a whitespace token (1) or just skip (0).
 * 
 * This function advances the input line past any leading whitespace, and if
 * 'flag' is set, it adds a whitespace token to the token list.
 */
void	skip_ec_t(t_global *global, int flag)
{
	int	i;

	i = 0;
	if (!global->line_input) // If there's no input, return immediately
		return ;

	// Skip spaces and tabs at the beginning of the input line
	while (global->line_input[i] && (global->line_input[i] == ' ' || global->line_input[i] == '\t'))
		i++;

	// Update the input line to skip past the whitespace
	update_line(&global->line_input, i, global);

	// If flag is set, add a whitespace token to the token list
	if (flag == 1)
	{
		// Duplicate a single space character as the token content
		global->content = ft_strdup(" ");
		if (!global->content)
			malloc_failed(global); // Handle allocation failure

		// Update the state and token type
		global->state = GENERAL;
		global->type = WHITE_SPACE;

		// Add the new token to the token list
		add_list(global);
	}
}

/**
 * check_state - Updates the parsing state based on the current character.
 * @global: Pointer to the global state structure.
 * 
 * This function checks if the current input character is a quote (' or ") and
 * toggles between GENERAL and quoted states (IN_DQUOTE for double quotes, 
 * IN_SQUOTE for single quotes). It is responsible for handling quoted strings.
 */
void	check_state(t_global *global)
{
	// Check for double quotes (")
	if (global->line_input[0] == '\"')
	{
		if (global->state == GENERAL)        // Switch to inside double quotes
			global->state = IN_DQUOTE;
		else if (global->state == IN_DQUOTE) // Close the double quotes, return to GENERAL state
		{
			global->state = GENERAL;
			global->type = WORD; // Mark the token type as a word
		}
	}

	// Check for single quotes (')
	if (global->line_input[0] == '\'')
	{
		if (global->state == GENERAL)        // Switch to inside single quotes
			global->state = IN_SQUOTE;
		else if (global->state == IN_SQUOTE) // Close the single quotes, return to GENERAL state
		{
			global->state = GENERAL;
			global->type = WORD; // Mark the token type as a word
		}
	}
}

/**
 * check_type - Determines the type of the current token based on its content.
 * @global: Pointer to the global state structure.
 * 
 * This function sets the token type based on the first character of the 
 * current token content (e.g., '>', '<', '|', or ' ').
 */
void	check_type(t_global *global)
{
	// Check for output redirection (>)
	if (global->content[0] == '>')
		global->type = REDIR_OUT;

	// Check for input redirection (<)
	if (global->content[0] == '<')
		global->type = REDIR_IN;

	// Check for pipe (|)
	if (global->content[0] == '|')
		global->type = PIPE;

	// Check for whitespace (space character)
	if (global->content[0] == ' ')
		global->type = WHITE_SPACE;
}

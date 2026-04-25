#include "bashy.h"

/**
 * create_node - Creates a new execution node and populates it with command 
 *               and redirection information from the token list.
 * @global: Pointer to the global state structure.
 * @node: Double pointer to the current position in the token list (t_lst).
 * 
 * This function processes a segment of the token list (up to a PIPE token or
 * the end of the list). It creates a new execution node for each command and 
 * links it to the global execution list. The function also handles input/output
 * redirections by creating redirection nodes and associating them with the 
 * current execution node.
 */
void	create_node(t_global *global, t_lst **node)
{
	t_redir		*new_rd_node;   // Pointer to the new redirection node
	t_exc_list	*new_exc_node;  // Pointer to the new execution node (command)

	// Create a new execution node for the command and add it to the execution list
	new_exc_node = new_exc_spot(global, CMD);
	add_back_exc(&global->root, new_exc_node);

	// Loop through tokens until a PIPE or end of the list is encountered
	while ((*node) && (*node)->type != PIPE)
	{
		// If the current token is a command argument (WORD or ENV), add it to the command list
		while (*node && ((*node)->type == WORD || (*node)->type == ENV))
			add_list_cmd(global, new_exc_node, node);

		// If the current token is a redirection (input or output)
		if (*node && ((*node)->type == REDIR_IN || (*node)->type == REDIR_OUT || (*node)->type == DREDIR_OUT))
		{
			// Create a new redirection node and add it to the current execution node's redirection list
			new_rd_node = new_vertex(global, *node);
			add_back_redir(&new_exc_node->redir, new_rd_node);

			// Skip to the next token after the redirection value
			*node = (*node)->next->next;

			// If there are more arguments (WORD or ENV) after the redirection, add them to the command list
			while (*node && ((*node)->type == WORD || (*node)->type == ENV))
				add_list_cmd(global, new_exc_node, node);
		}
	}
}

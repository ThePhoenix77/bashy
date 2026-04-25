#include "bashy.h"

// Create a new tree node and initialize its members
t_tree	*new_tree_node(t_global *global, t_type_node type)
{
	t_tree	*new;

	new = (t_tree *)malloc(sizeof(t_tree)); // Allocate memory for the new node
	if (!new)
		malloc_failed(global); // Handle memory allocation failure
	new->type = type; // Set the type of the node
	new->fds.in = -1; // Initialize file descriptors
	new->fds.out = -1;
	new->cmd_args = NULL; // Initialize command arguments
	new->redir = NULL; // Initialize redirection
	new->right = NULL; // Initialize right child
	new->left = NULL; // Initialize left child
	return (new);
}

// Free the entire tree recursively
void	free_tree(t_tree **root)
{
	if (*root == NULL)
		return; // Base case for recursion
	free_tree(&(*root)->left); // Free left subtree
	free_tree(&(*root)->right); // Free right subtree
	if ((*root)->cmd_args)
		free_2d_array(&(*root)->cmd_args); // Free command arguments if they exist
	if ((*root)->redir)
		free_redir_list(&(*root)->redir); // Free redirection list if it exists
	free(*root); // Free the current node
	*root = NULL; // Set the pointer to NULL
}

// Remove a node from the execution list
void	remove_node(t_exc_list **old)
{
	t_exc_list	*save;

	save = *old; // Save the current node to be removed
	*old = (*old)->next; // Move to the next node
	if (save->cmd_args)
		free_cmd_list(&save->cmd_args); // Free command arguments if they exist
	free(save); // Free the node
}

// Handle the last two nodes in the pipeline tree
void	last_nodes(t_global *global, t_tree *root, t_exc_list **end)
{
	root->left = new_tree_node(global, (*end)->type); // Create the left node
	if (*end && (*end)->cmd_args)
		root->left->cmd_args = get_args_arr(global,
				(*end)->cmd_args, (*end)->type); // Set command arguments
	if (*end && (*end)->redir)
	{
		root->left->redir = (*end)->redir; // Set redirection
		(*end)->redir = NULL; // Avoid double free
	}
	remove_node(end); // Remove the processed node
	global->exc_size--; // Decrement execution size
	root->right = new_tree_node(global, (*end)->type); // Create the right node
	if (*end && (*end)->cmd_args)
		root->right->cmd_args = get_args_arr(global,
				(*end)->cmd_args, (*end)->type); // Set command arguments
	if (*end && (*end)->redir)
	{
		root->right->redir = (*end)->redir; // Set redirection
		(*end)->redir = NULL; // Avoid double free
	}
	remove_node(end); // Remove the processed node
	global->exc_size--; // Decrement execution size
}

// Build a tree for the command execution structure
t_tree	*build_tree(t_global *global, t_exc_list **old)
{
	t_tree	*root;

	root = new_tree_node(global, PIPE_LINE); // Create the root node
	if (global->exc_size > 2)
	{
		root->left = new_tree_node(global, (*old)->type); // Create the left node
		if ((*old)->cmd_args)
			root->left->cmd_args = get_args_arr(global,
					(*old)->cmd_args, (*old)->type); // Set command arguments
		if ((*old)->redir)
		{
			root->left->redir = (*old)->redir; // Set redirection
			(*old)->redir = NULL; // Avoid double free
		}
		remove_node(old); // Remove the processed node
		global->exc_size--; // Decrement execution size
		root->right = build_tree(global, old); // Recursively build the right subtree
	}
	else
		last_nodes(global, root, old); // Handle the last two nodes
	return (root); // Return the constructed tree
}

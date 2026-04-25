#include "bashy.h"

// Add a new node to the pre_head list with the current global content
void	add_pre_list(t_global *global)
{
	t_lst	*new;

	new = new_node(global); // Create a new node
	new->content = ft_strdup(global->content); // Duplicate the global content
	if (!global->content) // Check if content duplication failed
	{
		free(new); // Free the newly allocated node
		malloc_failed(global); // Handle memory allocation failure
	}
	new->type = global->type; // Set the type of the new node
	new->state = global->state; // Set the state of the new node
	add_back(&global->pre_head, &new); // Add the new node to the pre_head list
	if (global->content) // If global content is not NULL
		free(global->content); // Free the global content
	global->content = NULL; // Set global content to NULL
}

// Split the environment variable content into separate words and store them in the pre_head list
void	split_env(t_global *global, t_lst *tmp)
{
	char	**spl_env; // Array to hold split environment variables
	int		i;

	i = 0;
	spl_env = ft_split(tmp->content, ' '); // Split the content by spaces
	if (!spl_env) // Check if splitting failed
		malloc_failed(global); // Handle memory allocation failure
	while (spl_env[i]) // Iterate over the split environment variables
	{
		global->content = ft_strdup(spl_env[i]); // Duplicate each split variable
		if (!global->content) // Check for duplication failure
			malloc_failed(global); // Handle memory allocation failure
		global->type = WORD; // Set the type to WORD
		global->state = GENERAL; // Set the state to GENERAL
		add_pre_list(global); // Add the new word to the pre_head list
		i++;
	}
	free_2d_array(&spl_env); // Free the array of split environment variables
}

// Join the content of the last node with the next node's content
bool	join_cont2(t_global *global, t_lst *tmp)
{
	t_lst	*last_pnode; // Pointer to the last node in pre_head
	int		len; // Length of the substring to join
	char	*save; // Buffer to hold the substring

	last_pnode = last_node(global->pre_head); // Get the last node
	if (!last_pnode) // If there is no last node, return FAILURE
		return (FAILURE);
	len = ft_strlen_un_char(tmp->next->content, ' '); // Get the length until the next space
	save = ft_substr(tmp->next->content, 0, len); // Get the substring to join
	if (!save) // Check if substring allocation failed
		malloc_failed(global); // Handle memory allocation failure
	update_line(&tmp->next->content, len, global); // Update the line by removing joined content
	global->content = ft_strjoin2(last_pnode->content, save); // Join the last node content with the substring
	if (!global->content) // Check if joining failed
		malloc_failed(global); // Handle memory allocation failure
	if (last_pnode) // If there is a last node, free its content
		free(last_pnode);
	if (save) // If the substring was allocated, free it
		free(save);
	last_pnode->content = global->content; // Update the last node's content with the joined content
	global->content = NULL; // Set global content to NULL
	return (SUCCESS); // Return SUCCESS
}

// Join the content of the current node with the next node based on type
int	join_cont(t_global *global, t_lst *tmp)
{
	t_lst	*node; // Pointer to the last node

	if (!tmp->next || tmp->next->state == GENERAL) // If there is no next node or it's in GENERAL state, return FAILURE
		return (FAILURE);
	if (tmp->next->type == WORD || tmp->next->type == ENV) // If the next node is a WORD or ENV type
	{
		node = last_node(global->pre_head); // Get the last node
		if (!node || !(node->content || tmp->next->content)) // If there is no last node or both contents are NULL, return FAILURE
			return (FAILURE);
		global->content = ft_strjoin2(node->content, tmp->next->content); // Join the contents
		if (!global->content) // Check if joining failed
			malloc_failed(global); // Handle memory allocation failure
		free(node->content); // Free the last node's content
		node->content = global->content; // Update the last node's content with the joined content
		return (3); // Return 3 as a status
	}
	if (tmp->next->type == ENV_SPL) // If the next node is an ENV_SPL type
		join_cont2(global, tmp); // Call the join_cont2 function
	return (SUCCESS); // Return SUCCESS
}

// Build the execution list from the pre_head list
void	bulid_list_exc(t_global *global)
{
	t_lst		*mv_node; // Pointer to traverse the pre_head list
	// t_lst		*save; // Pointer to save the initial position of mv_node

	mv_node = global->pre_head; // Initialize mv_node to the head of pre_head
	// save = global->pre_head; // Save the initial position of mv_node
	while (mv_node) // Iterate through the pre_head list
	{
		if (mv_node->type == PIPE) // If the node type is PIPE, skip to the next node
			mv_node = mv_node->next;
		create_node(global, &mv_node); // Create an execution node from the current mv_node
	}
}

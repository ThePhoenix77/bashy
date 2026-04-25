/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list_exp_fun.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eaboudi <eaboudi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/21 17:31:41 by eaboudi           #+#    #+#             */
/*   Updated: 2024/10/04 19:46:16 by eaboudi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bashy.h"

t_expand_list	*new_exp_node(char *content)
{
	t_expand_list	*new;

	new = (t_expand_list *)malloc(sizeof(t_expand_list));
	if (!new)
		return (NULL);
	new->content = content;
	new->next = NULL;
	new->prev = NULL;
	new->type = WORD;
	return (new);
}

t_expand_list	*last_exp_node(t_expand_list *head)
{
	while (head->next)
		head = head->next;
	return (head);
}

bool	add_exp_back(t_expand_list **head, t_expand_list *new)
{
	t_expand_list	*last_node;

	if (!new)
		return (FAILURE);
	if (!(*head))
		*head = new;
	else
	{
		last_node = last_exp_node(*head);
		new->prev = last_node;
		last_node->next = new;
	}
	return (SUCCESS);
}

void	free_exp_list(t_expand_list **head)
{
	t_expand_list	*tmp;

	if (!(*head))
		return ;
	while (*head)
	{
		tmp = *head;
		*head = (*head)->next;
		if (tmp->type != EMPTY_ENV && tmp->content != NULL)
			free(tmp->content);
		free(tmp);
	}
	*head = NULL;
}

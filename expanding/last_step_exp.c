/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   last_step_exp.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eaboudi <eaboudi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 11:00:47 by eaboudi           #+#    #+#             */
/*   Updated: 2024/10/05 15:38:46 by eaboudi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bashy.h"

void	expand_exit_status(t_global *global, t_expand_list *node)
{
	char	*value;
	char	*res;

	value = ft_itoa(global->exit_status);
	if (!value)
		malloc_failed(global);
	res = ft_strjoin2(value, node->content);
	if (!res)
	{
		free(value);
		malloc_failed(global);
	}
	if (node->content)
		free(node->content);
	node->content = res;
	res = NULL;
}

bool	special_expand(t_global *global)
{
	t_expand_list	*tmp;

	tmp = global->exp_head;
	while (tmp)
	{
		if (tmp->content[1] == '?')
		{
			update_line(&tmp->content, 2, global);
			expand_exit_status(global, tmp);
			tmp->type = WORD;
			return (SUCCESS);
		}
		if (ft_isdigit(tmp->content[1]))
		{
			update_line(&tmp->content, 2, global);
			tmp->type = WORD;
		}
		tmp = tmp->next;
	}
	return (FAILURE);
}

void	join_result(t_global *global)
{
	char			*tmp_str;
	t_expand_list	*tmp;

	tmp = global->exp_head;
	global->content = NULL;
	while (tmp)
	{
		if (!tmp->content && !global->content)
		{
			tmp = tmp->next;
			continue ;
		}
		tmp_str = ft_strjoin2(global->content, tmp->content);
		if (!tmp_str)
		{
			if (global->content)
				free(global->content);
			malloc_failed(global);
		}
		if (global->content)
			free(global->content);
		global->content = ft_strdup(tmp_str);
		free(tmp_str);
		tmp = tmp->next;
	}
}

void	finish_exp(t_global *global, t_lst **node)
{
	if ((*node)->content)
		free((*node)->content);
	join_result(global);
	(*node)->content = ft_strdup(global->content);
	if (global->content)
	{
		free(global->content);
		global->content = NULL;
	}
	free_exp_list(&global->exp_head);
}

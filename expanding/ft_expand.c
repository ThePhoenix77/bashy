/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_expand.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eaboudi <eaboudi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/21 17:52:20 by eaboudi           #+#    #+#             */
/*   Updated: 2024/10/05 15:39:11 by eaboudi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bashy.h"

void	check_bf_doll(t_global *global, t_lst **exp_node)
{
	int		len;
	char	*save;

	if (((*exp_node)->state == IN_DQUOTE || (*exp_node)->state == IN_SQUOTE)
		&& ((*exp_node)->prev && ((*exp_node)->prev->type == WORD
				|| (*exp_node)->prev->type == ENV)
			&& (*exp_node)->prev->state == GENERAL))
	{
		len = ft_strlen((*exp_node)->prev->content);
		if ((*exp_node)->prev->content[len - 1] == '$')
		{
			save = (*exp_node)->prev->content;
			(*exp_node)->prev->content = ft_substr(save, 0, len - 1);
			if (!(*exp_node)->prev->content)
			{
				free(save);
				malloc_failed(global);
			}
			free(save);
		}
	}
}

bool	main_exp_fun(t_global *global)
{
	t_lst	*tmp;

	tmp = global->head;
	while (tmp)
	{
		check_bf_doll(global, &tmp);
		if (tmp->type == ENV
			|| (tmp->type == EXP_HERE_DOC && tmp->state == GENERAL))
		{
			split_exp(global, &tmp->content);
			special_expand(global);
			update_var_name(global);
			transform_env(global);
			last_step_exp(global, tmp);
			finish_exp(global, &tmp);
		}
		tmp = tmp->next;
	}
	return (SUCCESS);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exp_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eaboudi <eaboudi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 09:55:56 by eaboudi           #+#    #+#             */
/*   Updated: 2024/10/05 15:38:22 by eaboudi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bashy.h"

void	add_exp_list(t_global *global, int len, char **line)
{
	t_expand_list	*new;
	char			*cont;

	cont = malloc(sizeof(char) * len + 1);
	if (!cont)
		malloc_failed(global);
	ft_strlcpy(cont, line[0], len + 1);
	new = new_exp_node(cont);
	if (!new)
		malloc_failed(global);
	add_exp_back(&global->exp_head, new);
	update_line(line, len, global);
}

bool	get_exp_var(t_global *global, char **line)
{
	t_expand_list	*last;
	int				i;

	i = 1;
	if (line[0][i] && ft_is_valid_expnd(line[0][i]))
	{
		while (line[0][i] && ft_is_valid_expnd(line[0][i]))
			i++;
		add_exp_list(global, i, line);
		last = last_exp_node(global->exp_head);
		last->type = ENV;
	}
	else
	{
		while (line[0][i] && line[0][i] != '$')
			i++;
		if (i == 1)
		{
			add_exp_list(global, 1, line);
			return (SUCCESS);
		}
		add_exp_list(global, i, line);
	}
	return (SUCCESS);
}

bool	split_exp(t_global *global, char **line)
{
	int				len_unt_doll;

	while (line[0] && line[0][0] != '\0')
	{
		len_unt_doll = ft_strlen_un_char(*line, '$');
		if (len_unt_doll == -1)
			break ;
		if (len_unt_doll == -2)
		{
			add_exp_list(global, ft_strlen(line[0]), line);
			return (SUCCESS);
		}
		else
		{
			if (len_unt_doll > 0)
				add_exp_list(global, len_unt_doll, line);
			get_exp_var(global, line);
		}
	}
	return (SUCCESS);
}

void	update_var_name(t_global *global)
{
	t_expand_list	*tmp;
	char			*save_ptr;

	tmp = global->exp_head;
	while (tmp)
	{
		if (tmp->type == ENV)
		{
			update_line(&tmp->content, 1, global);
			save_ptr = ft_strjoin(tmp->content, "=");
			if (!save_ptr)
				malloc_failed(global);
			free(tmp->content);
			tmp->content = save_ptr;
		}
		tmp = tmp->next;
	}
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exp_utils2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eaboudi <eaboudi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/25 16:10:04 by eaboudi           #+#    #+#             */
/*   Updated: 2024/10/05 15:38:31 by eaboudi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bashy.h"

void	transform_env(t_global *global)
{
	t_expand_list	*tmp;
	t_env_list		*env_var;

	tmp = global->exp_head;
	while (tmp)
	{
		if (tmp->type == ENV)
		{
			env_var = get_var_env(global, tmp->content);
			if (!env_var)
			{
				free(tmp->content);
				tmp->content = NULL;
				tmp->type = EMPTY_ENV;
			}
			else
			{
				free(tmp->content);
				tmp->content = ft_strdup(env_var->content);
				if (!tmp->content)
					malloc_failed(global);
			}
		}
		tmp = tmp->next;
	}
}

bool	check_exp_type(t_expand_list *node)
{
	int	len;

	if (node->prev && node->prev->type == WORD)
	{
		len = ft_strlen(node->prev->content);
		if (node->prev->prev && node->prev->prev->type == ENV && len == 1)
			return (FAILURE);
		if (node->prev->content[len - 1] == '=')
			return (SUCCESS);
	}
	return (FAILURE);
}

void	last_step_exp(t_global *global, t_lst *node)
{
	int				len;
	t_expand_list	*tmp;	

	tmp = global->exp_head;
	while (tmp)
	{
		if (tmp->type == ENV && tmp->content != NULL)
		{
			len = ft_strlen_un_char(tmp->content, '=') + 1;
			if (len == -2)
				return ;
			update_line(&tmp->content, len, global);
			if (node->state == GENERAL && ft_strchr(tmp->content, ' '))
				node->type = ENV_SPL;
		}
		if (node->state == GENERAL && tmp->type == ENV)
		{
			if (check_exp_type(tmp) == SUCCESS)
				node->type = ENV;
		}
		tmp = tmp->next;
	}
}

char	*ft_strjoin2(char const *s1, char const *s2)
{
	size_t	len1;
	size_t	len2;
	char	*str;

	if (!s1 && !s2)
		return (NULL);
	if (!s1)
		return (ft_strdup(s2));
	if (!s2)
		return (ft_strdup(s1));
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	str = malloc(sizeof(char) * (len1 + len2 + 1));
	if (!str)
		return (NULL);
	ft_strlcpy(str, s1, len1 + 1);
	ft_strlcat(str, s2, len1 + len2 + 1);
	return (str);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eaboudi <eaboudi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 18:29:10 by eaboudi           #+#    #+#             */
/*   Updated: 2024/10/05 11:48:33 by eaboudi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bashy.h"

char	*gen_file_name(t_global *global)
{
	char	*file_name;
	char	*num;

	while (1)
	{
		num = ft_itoa(global->here_doc_num);
		if (!num)
			malloc_failed(global);
		file_name = ft_strjoin2("/tmp/", num);
		free(num);
		if (!file_name)
			malloc_failed(global);
		if (access(file_name, F_OK) == -1)
			return (file_name);
		free(file_name);
		global->here_doc_num++;
	}
	return (NULL);
}

bool	open_file(t_global *global, t_lst *node)
{
	int		fd;
	char	*file_name;

	file_name = gen_file_name(global);
	fd = open(file_name, O_CREAT | O_RDWR, 0777);
	if (fd < 0)
	{
		perror("open");
		free(file_name);
		malloc_failed(global);
	}
	ft_putstr_fd(node->content, fd);
	if (close(fd) == -1)
	{
		perror("close:");
		free(file_name);
		malloc_failed(global);
	}
	if (node->content)
		free(node->content);
	node->content = file_name;
	file_name = NULL;
	return (SUCCESS);
}

void	open_heredoc_file(t_global *global)
{
	t_lst	*tmp;

	tmp = global->head;
	while (tmp)
	{
		if (tmp->type == HERE_DOC)
		{
			free(tmp->content);
			tmp->content = ft_strdup("<");
			if (!tmp->content)
				malloc_failed(global);
			tmp->type = REDIR_IN;
			if (tmp->next->type == EXP_HERE_DOC)
				tmp = tmp->next;
			else
				tmp = tmp->next->next;
			open_file(global, tmp);
			tmp->type = HERE_DOC_FILE;
		}
		tmp = tmp->next;
	}
}

void	empty_here_doc(t_global *global, t_lst *node)
{
	if (node->content)
		free(node->content);
	node->content = ft_strdup("");
	if (!node->content)
		malloc_failed(global);
}

void	handler(int sig)
{
	if (sig == SIGINT)
	{
		signal(SIGINT, SIG_IGN);
		close(0);
		init_sigaction();
	}
}

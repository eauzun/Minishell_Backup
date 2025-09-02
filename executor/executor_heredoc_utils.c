/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_heredoc_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emuzun <emuzun@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 16:03:08 by emuzun            #+#    #+#             */
/*   Updated: 2025/09/02 16:03:09 by emuzun           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_heredoc	*create_heredoc(char *delimiter, int expand_vars)
{
	t_heredoc	*heredoc;

	heredoc = malloc(sizeof(t_heredoc));
	if (!heredoc)
		return (NULL);
	heredoc->delimiter = ft_strdup(delimiter);
	if (!heredoc->delimiter)
	{
		free(heredoc);
		return (NULL);
	}
	heredoc->expand_vars = expand_vars;
	heredoc->pipe_fd[0] = -1;
	heredoc->pipe_fd[1] = -1;
	heredoc->next = NULL;
	return (heredoc);
}

void	free_heredocs(t_heredoc *heredocs)
{
	t_heredoc	*tmp;

	while (heredocs)
	{
		tmp = heredocs->next;
		if (heredocs->delimiter)
			free(heredocs->delimiter);
		if (heredocs->pipe_fd[0] != -1)
			close(heredocs->pipe_fd[0]);
		if (heredocs->pipe_fd[1] != -1)
			close(heredocs->pipe_fd[1]);
		free(heredocs);
		heredocs = tmp;
	}
}

void	add_heredoc(t_command *cmd, char *delimiter, int expand_vars)
{
	t_heredoc	*new_heredoc;
	t_heredoc	*cur;

	new_heredoc = create_heredoc(delimiter, expand_vars);
	if (!new_heredoc)
		return ;
	if (!cmd->heredocs)
		cmd->heredocs = new_heredoc;
	else
	{
		cur = cmd->heredocs;
		while (cur->next)
			cur = cur->next;
		cur->next = new_heredoc;
	}
}

void	cleanup_heredoc_interrupt(t_command *cmd)
{
	if (cmd && cmd->heredocs)
	{
		free_heredocs(cmd->heredocs);
		cmd->heredocs = NULL;
	}
}

void	ft_putstr_fd(char *s, int fd)
{
	if (!s)
		return ;
	write(fd, s, ft_strlen(s));
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_heredoc_utils2.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emuzun <emuzun@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 16:03:11 by emuzun            #+#    #+#             */
/*   Updated: 2025/09/02 16:03:12 by emuzun           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	heredoc_eof_warning(char *delimiter)
{
	write(2,
		"warning: here-document delimited by end-of-file (wanted `",
		58);
	write(2, delimiter, ft_strlen(delimiter));
	write(2, "')\n", 3);
	return (0);
}

static int	handle_line_expand(char **line, char **env)
{
	char	*expanded;

	expanded = expand_string(*line, env);
	free(*line);
	if (!expanded)
		return (1);
	*line = expanded;
	return (0);
}

static int	check_exit_or_delim(char *line, char *delimiter, int initial)
{
	if (g_exit_code(-1) == 130)
		return (130);
	if (ft_strcmp(line, delimiter) == 0)
	{
		free(line);
		g_exit_code(initial);
		return (1);
	}
	return (0);
}

static void	write_and_free_line(int fd, char *line)
{
	write(fd, line, ft_strlen(line));
	write(fd, "\n", 1);
	free(line);
}

int	read_heredoc_line(char *delimiter, int expand, char **env, int write_fd)
{
	char	*line;
	int		initial;

	initial = g_exit_code(-1);
	while (1)
	{
		line = readline("> ");
		if (!line)
			return (heredoc_eof_warning(delimiter));
		if (check_exit_or_delim(line, delimiter, initial))
			return (0);
		if (expand && handle_line_expand(&line, env))
			return (1);
		write_and_free_line(write_fd, line);
	}
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emuzun <emuzun@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 16:04:01 by emuzun            #+#    #+#             */
/*   Updated: 2025/09/02 16:04:02 by emuzun           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	syntax_error(const char *msg)
{
	write(2, msg, ft_strlen(msg));
	g_exit_code(258);
}

static int	count_args(char **args)
{
	int	count;

	count = 0;
	while (args && args[count])
		count++;
	return (count);
}

void	add_argument(t_command *cmd, char *arg)
{
	char	**new_args;
	int		count;
	int		i;

	if (!cmd || !arg)
		return ;
	count = count_args(cmd->args);
	new_args = malloc(sizeof(char *) * (count + 2));
	if (!new_args)
		return ;
	i = 0;
	while (i < count)
	{
		new_args[i] = cmd->args[i];
		i++;
	}
	new_args[i] = ft_strdup(arg);
	if (!new_args[i])
	{
		free(new_args);
		return ;
	}
	new_args[i + 1] = NULL;
	free(cmd->args);
	cmd->args = new_args;
}

static void	add_redir_to_list(t_command *cmd, t_redir *new_redir)
{
	t_redir	*cur;

	if (!cmd->redirs)
		cmd->redirs = new_redir;
	else
	{
		cur = cmd->redirs;
		while (cur->next)
			cur = cur->next;
		cur->next = new_redir;
	}
}

void	add_redir(t_command *cmd, t_redir_type type, char *file)
{
	t_redir	*new_redir;

	if (!cmd || !file)
	{
		free(file);
		return ;
	}
	new_redir = (t_redir *)malloc(sizeof(*new_redir));
	if (!new_redir)
	{
		free(file);
		return ;
	}
	new_redir->type = type;
	new_redir->file = file;
	new_redir->next = NULL;
	add_redir_to_list(cmd, new_redir);
}

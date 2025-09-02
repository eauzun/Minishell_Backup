/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emuzun <emuzun@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 16:03:53 by emuzun            #+#    #+#             */
/*   Updated: 2025/09/02 16:03:54 by emuzun           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_command	*create_command(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->args = malloc(sizeof(char *) * 1);
	if (!cmd->args)
	{
		free(cmd);
		return (NULL);
	}
	cmd->args[0] = NULL;
	cmd->infile = NULL;
	cmd->outfile = NULL;
	cmd->append = 0;
	cmd->heredoc = 0;
	cmd->redir_error = 0;
	cmd->redirs = NULL;
	cmd->heredocs = NULL;
	cmd->next = NULL;
	return (cmd);
}

void	link_commands(t_command **head, t_command *new_cmd)
{
	t_command	*cur;

	if (!*head)
		*head = new_cmd;
	else
	{
		cur = *head;
		while (cur->next)
			cur = cur->next;
		cur->next = new_cmd;
	}
}

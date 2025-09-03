/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hialpagu <hialpagu@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 16:04:07 by emuzun            #+#    #+#             */
/*   Updated: 2025/09/03 08:52:03 by hialpagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	handle_word(t_token **cur, t_command *cmd)
{
	char	*merged;

	merged = create_word(cur);
	if (!merged)
		return (0);
	add_argument(cmd, merged);
	free(merged);
	return (1);
}

static t_command	*parse_single_command(t_token **tokens)
{
	t_command	*cmd;
	t_token		*cur;

	cmd = create_command();
	if (!cmd)
		return (NULL);
	cur = *tokens;
	while (cur && cur->type != T_PIPE)
	{
		if (cur->type >= T_REDIR_IN && cur->type <= T_HEREDOC)
		{
			if (!handle_redirection(&cur, cmd))
				return (free_commands(cmd), NULL);
		}
		else if (is_word(cur))
		{
			if (!handle_word(&cur, cmd))
				return (free_commands(cmd), NULL);
		}
		else
			cur = cur->next;
	}
	*tokens = cur;
	return (cmd);
}

t_command	*parse_tokens(t_token *tokens)
{
	t_command	*cmd_list;
	t_command	*cmd;

	cmd_list = NULL;
	if (!tokens)
		return (NULL);
	if (validate_syntax(tokens))
	{
		g_exit_code(2);
		return (NULL);
	}
	while (tokens)
	{
		cmd = parse_single_command(&tokens);
		if (!cmd)
		{
			free_commands(cmd_list);
			return (NULL);
		}
		link_commands(&cmd_list, cmd);
		if (tokens && tokens->type == T_PIPE)
			tokens = tokens->next;
	}
	return (cmd_list);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emuzun <emuzun@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 16:03:58 by emuzun            #+#    #+#             */
/*   Updated: 2025/09/02 16:03:59 by emuzun           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*join_filename_tokens(t_token **cur, int *was_quoted)
{
	t_token	*cur_t;
	char	*acc;

	cur_t = (*cur)->next;
	if (!is_word(cur_t))
	{
		syntax_error("syntax error: unexpected token\n");
		return (NULL);
	}
	if (was_quoted)
		*was_quoted = (cur_t->type != T_WORD);
	acc = collect_filename_words(&cur_t);
	if (!acc)
		return (NULL);
	*cur = cur_t;
	return (acc);
}

static int	get_redir_target(t_token **cur, char **out_name, int *was_quoted)
{
	*out_name = join_filename_tokens(cur, was_quoted);
	if (!*out_name)
		return (0);
	return (1);
}

static void	apply_heredoc(t_command *cmd, char *name, int quoted_first_part)
{
	add_heredoc(cmd, name, !quoted_first_part);
	cmd->heredoc = 1;
	free(name);
}

void	parse_redirections(t_token **cur, t_command *cmd)
{
	t_token_type	redir_type;
	char			*name;
	int				is_quoted;

	redir_type = (*cur)->type;
	if (redir_type == T_HEREDOC)
	{
		is_quoted = 0;
		if (!get_redir_target(cur, &name, &is_quoted))
			return ;
		apply_heredoc(cmd, name, is_quoted);
		return ;
	}
	if (!get_redir_target(cur, &name, NULL))
		return ;
	if (redir_type == T_REDIR_IN)
	{
		add_redir(cmd, R_IN, name);
		cmd->heredoc = 0;
	}
	else if (redir_type == T_REDIR_OUT)
		add_redir(cmd, R_OUT, name);
	else if (redir_type == T_APPEND)
		add_redir(cmd, R_APPEND, name);
}

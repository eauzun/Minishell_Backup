#include "../minishell.h"

char	*join_filename_tokens(t_token **cur, int *was_quoted)
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

int	get_redir_target(t_token **cur, char **out_name, int *was_quoted)
{
	*out_name = join_filename_tokens(cur, was_quoted);
	if (!*out_name)
		return (0);
	return (1);
}

void	apply_heredoc(t_command *cmd, char *name, int quoted_first_part)
{
	if (cmd->heredoc_prev)
	{
		free(cmd->heredoc_prev);
		cmd->heredoc_prev = NULL;
	}
	cmd->heredoc_prev = cmd->infile;
	cmd->infile = name;
	cmd->heredoc = 1;
	cmd->heredoc_expand = (quoted_first_part == 0);
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

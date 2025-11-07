#include "../minishell.h"

int	get_redir_target(t_token **cur, char **out_name, int *was_quoted)
{
	t_token	*cur_t;
	char	*acc;

	cur_t = (*cur)->next;
	if (!is_word(cur_t))
	{
		syntax_error("syntax error: unexpected token\n");
		return (0);
	}
	if (was_quoted)
	{
    	if (cur_t->type == T_WORD_SINGLE || cur_t->type == T_WORD_DOUBLE)
    	    *was_quoted = 1;
    	else
    	    *was_quoted = 0;
	}
	acc = create_word(&cur_t);
	if (!acc)
		return (0);
	*cur = cur_t;
	*out_name = acc;
	return (1);
}

static void	apply_heredoc(t_command *cmd, char *name, int quoted)
{
	add_heredoc(cmd, name, !quoted);
	cmd->heredoc = 1;
	free(name);
}

int	handle_redirection(t_token **cur, t_command *cmd)
{
	t_token_type	redir_type;
	char			*name;
	int				is_quoted;

	redir_type = (*cur)->type;
	if (redir_type == T_HEREDOC)
	{
		is_quoted = 0;
		if (!get_redir_target(cur, &name, &is_quoted))
			return (0);
		apply_heredoc(cmd, name, is_quoted);
		return (1);
	}
	if (!get_redir_target(cur, &name, NULL))
		return (0);
	if (redir_type == T_REDIR_IN)
	{
		add_redir(cmd, R_IN, name);
		cmd->heredoc = 0;
	}
	else if (redir_type == T_REDIR_OUT)
		add_redir(cmd, R_OUT, name);
	else if (redir_type == T_APPEND)
		add_redir(cmd, R_APPEND, name);
	return (1);
}

#include "../minishell.h"

int	check_pipe_syntax(t_token *cur)
{
	if (!cur->next || cur->next->type == T_PIPE)
	{
		syntax_error("syntax error: unexpected token '|'\n");
		return (1);
	}
	return (0);
}

int	check_redir_syntax(t_token *cur)
{
	if (!is_word(cur->next))
	{
		if (!cur->next)
			syntax_error("syntax error: unexpected token 'newline'\n");
		else if (cur->next->type == T_PIPE)
			syntax_error("syntax error: unexpected token '|'\n");
		else
			syntax_error("syntax error: unexpected token\n");
		return (1);
	}
	return (0);
}

int	check_first_token(t_token *tokens)
{
	if (tokens->type == T_PIPE)
	{
		syntax_error("syntax error: unexpected token '|'\n");
		return (1);
	}
	return (0);
}

int	validate_syntax(t_token *tokens)
{
	t_token	*cur;

	if (!tokens)
		return (0);
	if (check_first_token(tokens))
		return (1);
	cur = tokens;
	while (cur)
	{
		if (cur->type == T_PIPE)
		{
			if (check_pipe_syntax(cur))
				return (1);
		}
		else if (cur->type >= T_REDIR_IN && cur->type <= T_HEREDOC)
		{
			if (check_redir_syntax(cur))
				return (1);
		}
		cur = cur->next;
	}
	return (0);
}

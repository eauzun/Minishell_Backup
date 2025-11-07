#include "../minishell.h"

t_token	*single_quote(const char *line, size_t *i)
{
	t_token	*token;
	size_t	start;

	start = ++(*i);
	while (line[*i] && line[*i] != '\'')
		(*i)++;
	if (line[*i] != '\'')
	{
		write(2, "syntax error: unclosed single quote\n", 36);
		return (NULL);
	}
	token = new_token(T_WORD_SINGLE, &line[start], *i - start);
	(*i)++;
	return (token);
}

t_token	*double_quote(const char *line, size_t *i)
{
	t_token	*token;
	size_t	start;

	start = ++(*i);
	while (line[*i] && line[*i] != '"')
		(*i)++;
	if (line[*i] != '"')
	{
		write(2, "syntax error: unclosed double quote\n", 36);
		return (NULL);
	}
	token = new_token(T_WORD_DOUBLE, &line[start], *i - start);
	(*i)++;
	return (token);
}

t_token	*redirections(const char *line, size_t *i)
{
	t_token_type	symbol;
	size_t			start;

	start = *i;
	if ((line[*i] == '>' && line[*i + 1] == '>')
		|| (line[*i] == '<' && line[*i + 1] == '<'))
	{
		if (line[*i] == '>')
			symbol = T_APPEND;
		else
			symbol = T_HEREDOC;
		*i += 2;
	}
	else if (line[*i] == '>' || line[*i] == '<')
	{
		if (line[*i] == '>')
			symbol = T_REDIR_OUT;
		else
			symbol = T_REDIR_IN;
		*i += 1;
	}
	else
		return (NULL);
	return (new_token(symbol, &line[start], *i - start));
}

t_token	*is_pipe(const char *line, size_t *i)
{
	t_token_type	symbol;
	t_token			*token;
	size_t			len;

	symbol = T_PIPE;
	len = 1;
	token = new_token(symbol, &line[*i], len);
	*i += len;
	return (token);
}

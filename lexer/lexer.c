#include "../minishell.h"

t_token	*handle_special(const char *line, size_t *i)
{
	if (line[*i] == '\'')
		return (single_quote(line, i));
	else if (line[*i] == '"')
		return (double_quote(line, i));
	else if (line[*i] == '<' || line[*i] == '>')
		return (redirections(line, i));
	else if (line[*i] == '|')
		return (is_pipe(line, i));
	return (NULL);
}

t_token	*token_word(const char *line, size_t *i)
{
	size_t	start;

	start = *i;
	while (line[*i] && line[*i] != ' ' && line[*i] != '\t' && line[*i] != '\n'
		&& line[*i] != '|' && line[*i] != '<' && line[*i] != '>'
		&& line[*i] != '\'' && line[*i] != '"')
		(*i)++;
	return (new_token(T_WORD, &line[start], *i - start));
}

static t_token	*lexer_one(const char *line, size_t *i, int *had_ws, int *done)
{
	t_token	*token;
	size_t	prev;

	*done = 0;
	prev = *i;
	skip_whitespace(line, i);
	*had_ws = (*i != prev);
	if (!line[*i])
	{
		*done = 1;
		return (NULL);
	}
	if (line[*i] == '\'' || line[*i] == '"' || line[*i] == '<'
		|| line[*i] == '>' || line[*i] == '|')
		token = handle_special(line, i);
	else
		token = token_word(line, i);
	return (token);
}

t_token	*lexer_init(const char *line)
{
	t_token	*tokens;
	t_token	*tok;
	size_t	i;
	int		had_ws;
	int		done;

	tokens = NULL;
	i = 0;
	while (1)
	{
		tok = lexer_one(line, &i, &had_ws, &done);
		if (done)
			break ;
		if (!tok)
		{
			free_token(tokens);
			return (NULL);
		}
		tok->sep_ws = had_ws;
		add_token(&tokens, tok);
	}
	return (tokens);
}

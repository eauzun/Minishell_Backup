#include "../minishell.h"

void	skip_whitespace(const char *line, size_t *i)
{
	while (line[*i] == ' ' || line[*i] == '\t' || line[*i] == '\n')
		(*i)++;
}

void	free_token(t_token *list)
{
	t_token	*tmp;

	while (list)
	{
		tmp = list->next;
		free(list->str);
		free(list);
		list = tmp;
	}
}

void	add_token(t_token **list, t_token *token)
{
	t_token	*cur;

	if (!*list)
		*list = token;
	else
	{
		cur = *list;
		while (cur->next)
			cur = cur->next;
		cur->next = token;
	}
}

t_token	*new_token(t_token_type type, const char *start, size_t len)
{
	t_token	*token;
	size_t	i;

	i = 0;
	token = malloc(sizeof(*token));
	if (!token)
		return (NULL);
	token->str = malloc(sizeof(char) * (len + 1));
	if (!token->str)
	{
		free(token);
		return (NULL);
	}
	while (i < len)
	{
		token->str[i] = start[i];
		i++;
	}
	token->str[i] = '\0';
	token->type = type;
	token->sep_ws = 0;
	token->next = NULL;
	return (token);
}

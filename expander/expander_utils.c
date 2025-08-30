#include "../minishell.h"

char	*quoted_next_segment(const char *str, size_t *i, char **env, char quote)
{
	size_t	start;

	if (quote == '"' && str[*i] == '$')
		return (handle_dollar((char *)str, i, env));
	start = *i;
	if (quote == '"')
	{
		while (str[*i] && str[*i] != quote && str[*i] != '$')
			(*i)++;
	}
	else
		while (str[*i] && str[*i] != quote)
			(*i)++;
	return (ft_substr(str, start, *i - start));
}

char	*expand_in_quotes(const char *str, size_t *i, char **env, char quote)
{
	char	*result;
	char	*part;

	result = ft_strdup("");
	if (!result)
		return (NULL);
	(*i)++;
	while (str[*i] && str[*i] != quote)
	{
		part = quoted_next_segment(str, i, env, quote);
		if (!part)
		{
			free(result);
			return (NULL);
		}
		result = join_and_free(result, part);
		if (!result)
			return (NULL);
	}
	if (str[*i] == quote)
		(*i)++;
	return (result);
}

char	*expand_next_segment(const char *str, size_t *i, char **env)
{
	char	*tmp;
	size_t	start;

	if (str[*i] == '\'' || str[*i] == '"')
		return (expand_in_quotes(str, i, env, str[*i]));
	if (str[*i] == '$')
		return (handle_dollar((char *)str, i, env));
	start = *i;
	while (str[*i] && str[*i] != '$' && str[*i] != '\'' && str[*i] != '"')
		(*i)++;
	tmp = ft_substr(str, start, *i - start);
	return (tmp);
}

char	*expand_string(const char *str, char **env)
{
	char	*result;
	char	*seg;
	size_t	i;

	i = 0;
	result = ft_strdup("");
	if (!result)
		return (NULL);
	while (str[i])
	{
		seg = expand_next_segment(str, &i, env);
		if (!seg)
		{
			free(result);
			return (NULL);
		}
		result = join_and_free(result, seg);
		if (!result)
			return (NULL);
	}
	return (result);
}

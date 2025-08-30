#include "../minishell.h"

char	*expand_single_quote(char *str)
{
	return (ft_strdup(str));
}

static char	*double_quote_part(const char *str, size_t *i, char **env)
{
	char	*part;
	size_t	start;

	if (str[*i] == '$')
		return (handle_dollar((char *)str, i, env));
	start = *i;
	while (str[*i] && str[*i] != '$')
		(*i)++;
	part = ft_substr(str, start, *i - start);
	return (part);
}

char	*expand_double_quote(const char *str, char **env)
{
	char	*res;
	char	*part;
	char	*joined;
	size_t	i;

	i = 0;
	res = ft_strdup("");
	if (!res)
		return (NULL);
	while (str[i])
	{
		part = double_quote_part(str, &i, env);
		if (!part)
		{
			free(res);
			return (NULL);
		}
		joined = ft_strjoin(res, part);
		free(res);
		free(part);
		if (!joined)
			return (NULL);
		res = joined;
	}
	return (res);
}

char	*expand_regular_word(char *str, char **env)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '$')
			return (expand_string(str, env));
		i++;
	}
	return (ft_strdup(str));
}

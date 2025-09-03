/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_type.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hialpagu <hialpagu@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 16:03:38 by emuzun            #+#    #+#             */
/*   Updated: 2025/09/03 05:23:41 by hialpagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*next_segment(const char *str, size_t *i, char **env)
{
	size_t	start;

	if (str[*i] == '$')
		return (handle_dollar((char *)str, i, env));
	start = *i;
	while (str[*i] && str[*i] != '$')
		(*i)++;
	return (ft_substr(str, start, *i - start));
}

char	*expand_string(const char *str, char **env)
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
		part = next_segment(str, &i, env);
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

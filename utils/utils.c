#include "../minishell.h"

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

char	*ft_strdup(const char *s)
{
	char	*dup;
	size_t	len;
	size_t	i;

	len = ft_strlen(s);
	dup = malloc(sizeof(char) * (len + 1));
	if (!dup)
		return (NULL);
	i = 0;
	while (i < len)
	{
		dup[i] = s[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

bool	is_only_space(const char *s)
{
	if (!s)
		return (true);
	while (*s)
	{
		if (*s != ' ' && (*s < 9 || *s > 13))
			return (false);
		s++;
	}
	return (true);
}

char	*collect_filename_words(t_token **cur_t)
{
	char	*acc;
	char	*tmp;
	int		first;

	acc = ft_strdup("");
	if (!acc)
		return (NULL);
	first = 1;
	while (*cur_t && is_word(*cur_t) && (first || (*cur_t)->sep_ws == 0))
	{
		tmp = ft_strjoin(acc, (*cur_t)->str);
		free(acc);
		if (!tmp)
			return (NULL);
		acc = tmp;
		*cur_t = (*cur_t)->next;
		first = 0;
	}
	return (acc);
}

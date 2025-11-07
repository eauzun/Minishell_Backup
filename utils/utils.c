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

char	*create_word(t_token **cur)
{
	char	*acc;
	char	*tmp;
	t_token	*t;
	int		first;

	t = *cur;
	acc = ft_strdup("");
	if (!acc)
		return (NULL);
	first = 1;
	while (t && is_word(t) && (first || t->sep_ws == 0))
	{
		tmp = ft_strjoin(acc, t->str);
		free(acc);
		if (!tmp)
			return (NULL);
		acc = tmp;
		t = t->next;
		first = 0;
	}
	*cur = t;
	return (acc);
}


size_t	ft_strcpy(char *dst, const char *src)
{
	size_t	i;

	i = 0;
	while (src[i] != '\0')
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return (i);
}

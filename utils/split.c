#include "../minishell.h"

static int	count_words(const char *s, char c)
{
	int	count;

	count = 0;
	while (*s)
	{
		while (*s && *s == c)
			s++;
		if (*s)
		{
			count++;
			while (*s && *s != c)
				s++;
		}
	}
	return (count);
}

static void	*free_partial(char **arr, int i)
{
	while (i >= 0)
		free(arr[i--]);
	free(arr);
	return (NULL);
}

static int	fill_word(char ***result, const char **s, char c, int i)
{
	int	len;

	while (**s && **s == c)
		(*s)++;
	len = 0;
	while ((*s)[len] && (*s)[len] != c)
		len++;
	(*result)[i] = ft_substr(*s, 0, len);
	if (!(*result)[i])
		return (0);
	*s += len;
	return (1);
}

char	**ft_split(const char *s, char c)
{
	char	**result;
	int		words;
	int		i;

	if (!s)
		return (NULL);
	words = count_words(s, c);
	result = malloc(sizeof(char *) * (words + 1));
	if (!result)
		return (NULL);
	i = 0;
	while (i < words)
	{
		if (!fill_word(&result, &s, c, i))
			return (free_partial(result, i - 1));
		i++;
	}
	result[i] = NULL;
	return (result);
}

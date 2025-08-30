#include "../minishell.h"

static int	total(long c)
{
	int	i;

	i = 0;
	if (c < 0)
	{
		i++;
		c *= -1;
	}
	if (c == 0)
		return (1);
	while (c != 0)
	{
		c = c / 10;
		i++;
	}
	return (i);
}

char	*ft_itoa(int n)
{
	char	*arr;
	size_t	i;
	long	num;

	i = total(n);
	num = (long)n;
	arr = malloc(sizeof(char) * (total(num) + 1));
	if (arr == NULL)
		return (NULL);
	if (num == 0)
		arr[0] = '0';
	arr[i] = '\0';
	while (num != 0)
	{
		if (num < 0)
		{
			arr[0] = '-';
			num = -num;
		}
		arr[--i] = (num % 10) + 48;
		num = num / 10;
	}
	return (arr);
}

int	ft_isalnum(int c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
		|| (c >= '0' && c <= '9'))
		return (1);
	return (0);
}

char	*ft_strchr(const char *s, int c)
{
	size_t	i;

	i = 0;
	while (s[i])
	{
		if (s[i] == (char)c)
		{
			return ((char *)s + i);
		}
		i++;
	}
	if (s[i] == (char)c)
		return ((char *)s + i);
	return (NULL);
}

#include "../minishell.h"


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

size_t	ft_strncpy(char *dst, const char *src, size_t n)
{
    size_t	i;  
    i = 0;
    while (i < n && src[i] != '\0')
    {
        dst[i] = src[i];
        i++;
    }
    while (i < n)
    {
        dst[i] = '\0';
        i++;
    }
    return (n);
}
char	*ft_strrchr(const char *str, int c)
{
	size_t	i;
	char	*ptr;

	i = 0;
	ptr = 0;
	while (str[i])
	{
		if (str[i] == (char)c)
			ptr = (char *)(str + i);
		i++;
	}
	if (str[i] == (char)c)
		ptr = (char *)(str + i);
	return (ptr);
}
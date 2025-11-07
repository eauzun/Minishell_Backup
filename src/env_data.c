#include "../minishell.h"

char	**cpy_env_data(char **env)
{
	int		i;
	char	**arr;

	i = 0;
	while (env[i])
		i++;
	arr = malloc(sizeof(char *) * (i + 1));
	if (!arr)
		return (NULL);
	i = 0;
	while (env[i])
	{
		arr[i] = ft_strdup(env[i]);
		if (!arr[i])
		{
			while (--i >= 0)
				free(arr[i]);
			free(arr);
			return (NULL);
		}
		i++;
	}
	arr[i] = NULL;
	return (arr);
}

void	free_env_data(char **env)
{
	int	i;

	if (!env)
		return ;
	i = 0;
	while (env[i])
	{
		free(env[i]);
		i++;
	}
	free(env);
}

int	g_exit_code(int code)
{
	static int	temp = 0;

	if (code != -1)
		temp = code;
	return (temp);
}

int	check_should_exit(void)
{
	int	code;

	code = g_exit_code(-1);
	return (code >= 256);
}

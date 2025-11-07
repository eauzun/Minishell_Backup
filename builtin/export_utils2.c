#include "../minishell.h"

void	free_env_array(char **env)
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

static void	sort_env_array(char **arr, int size)
{
	int		i;
	int		j;
	char	*temp;

	i = 0;
	while (i < size - 1)
	{
		j = 0;
		while (j < size - i - 1)
		{
			if (ft_strcmp(arr[j], arr[j + 1]) > 0)
			{
				temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
			j++;
		}
		i++;
	}
}

char	**copy_env_array(char **env)
{
	char	**copy;
	int		size;
	int		i;

	size = get_env_size(env);
	copy = malloc(sizeof(char *) * (size + 1));
	if (!copy)
		return (NULL);
	i = 0;
	while (i < size)
	{
		copy[i] = ft_strdup(env[i]);
		if (!copy[i])
		{
			free_env_array(copy);
			return (NULL);
		}
		i++;
	}
	copy[size] = NULL;
	return (copy);
}

char	**sort_env_for_export(char **env)
{
	char	**sorted_env;
	int		size;

	sorted_env = copy_env_array(env);
	if (!sorted_env)
		return (NULL);
	size = get_env_size(sorted_env);
	sort_env_array(sorted_env, size);
	return (sorted_env);
}

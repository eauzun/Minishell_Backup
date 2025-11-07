#include "../minishell.h"

int	find_env_var(char **env, char *name)
{
	int		i;
	size_t	name_len;
	size_t	env_len;

	if (!env || !name)
		return (-1);
	name_len = ft_strlen(name);
	if (name_len == 0)
		return (-1);
	i = 0;
	while (env[i])
	{
		if (!env[i])
			break ;
		env_len = ft_strlen(env[i]);
		if (env_len > name_len && ft_strncmp(env[i], name, name_len) == 0
			&& env[i][name_len] == '=')
			return (i);
		i++;
	}
	return (-1);
}

int	update_env_var(char ***env, int pos, char *name, char *value)
{
	char	*new_var;
	char	*temp;

	temp = ft_strjoin(name, "=");
	if (!temp)
		return (1);
	new_var = ft_strjoin(temp, value);
	free(temp);
	if (!new_var)
		return (1);
	free((*env)[pos]);
	(*env)[pos] = new_var;
	return (0);
}

char	*create_env_string(char *name, char *value)
{
	char	*temp;
	char	*result;

	if (!name)
		return (NULL);
	if (!value)
		return (ft_strdup(name));
	temp = ft_strjoin(name, "=");
	if (!temp)
		return (NULL);
	result = ft_strjoin(temp, value);
	free(temp);
	return (result);
}

int	get_env_size(char **env)
{
	int	size;

	size = 0;
	while (env && env[size])
		size++;
	return (size);
}

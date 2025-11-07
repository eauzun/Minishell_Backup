#include "../minishell.h"

int	is_valid_identifier(char *str)
{
	int	i;
	int	eq_pos;

	if (!str || str[0] == '\0')
		return (0);
	if (str[0] >= '0' && str[0] <= '9')
		return (0);
	eq_pos = find_equals_position(str);
	if (eq_pos == 0)
		return (0);
	i = 0;
	while (i < eq_pos || (eq_pos == -1 && str[i]))
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

int	find_equals_position(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '=')
			return (i);
		i++;
	}
	return (-1);
}

void	parse_assignment(char *arg, char **name, char **value)
{
	int	eq_pos;

	*name = NULL;
	*value = NULL;
	eq_pos = find_equals_position(arg);
	if (eq_pos == -1)
	{
		*name = ft_strdup(arg);
		*value = NULL;
	}
	else
	{
		*name = ft_substr(arg, 0, eq_pos);
		*value = ft_strdup(arg + eq_pos + 1);
	}
}

int	set_env_var(char ***env, char *name, char *value)
{
	char	**new_env;
	int		pos;

	pos = find_env_var(*env, name);
	if (pos != -1)
	{
		if (value)
			return (update_env_var(env, pos, name, value));
		return (0);
	}
	else
	{
		new_env = add_env_var(*env, name, value);
		if (!new_env)
			return (1);
		free_env_array(*env);
		*env = new_env;
		return (0);
	}
}

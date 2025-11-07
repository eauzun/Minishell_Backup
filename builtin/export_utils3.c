#include "../minishell.h"

int	print_export_line(char *line)
{
	if (write(1, "declare -x ", 11) == -1)
		return (1);
	if (write(1, line, ft_strlen(line)) == -1)
		return (1);
	if (write(1, "\n", 1) == -1)
		return (1);
	return (0);
}

static int	dup_env_into(char **dst, char **src, int size)
{
	int	i;

	i = 0;
	while (i < size)
	{
		dst[i] = ft_strdup(src[i]);
		if (!dst[i])
			return (0);
		i++;
	}
	dst[i] = NULL;
	return (1);
}

char	**add_env_var(char **env, char *name, char *value)
{
	char	**new_env;
	char	*new_var;
	int		size;

	size = get_env_size(env);
	new_env = malloc(sizeof(char *) * (size + 2));
	if (!new_env)
		return (NULL);
	if (!dup_env_into(new_env, env, size))
	{
		free_env_array(new_env);
		return (NULL);
	}
	new_var = create_env_string(name, value);
	if (!new_var)
	{
		free_env_array(new_env);
		return (NULL);
	}
	new_env[size] = new_var;
	new_env[size + 1] = NULL;
	return (new_env);
}

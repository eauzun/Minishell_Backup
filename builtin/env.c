#include "../minishell.h"

int	builtin_env(char **env)
{
	int	i;

	if (!env)
		return (1);
	i = 0;
	while (env[i])
	{
		if (write(1, env[i], ft_strlen(env[i])) == -1)
			return (1);
		if (write(1, "\n", 1) == -1)
			return (1);
		i++;
	}
	return (0);
}

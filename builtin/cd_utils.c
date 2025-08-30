#include "../minishell.h"

char	*get_home_path(char **env)
{
	char	*home;

	home = get_env_value("HOME", env);
	if (!home)
	{
		if (write(2, "minishell: cd: HOME not set\n", 28) == -1)
			return (NULL);
		return (NULL);
	}
	return (ft_strdup(home));
}

char	*get_oldpwd_path(char **env)
{
	char	*oldpwd;

	oldpwd = get_env_value("OLDPWD", env);
	if (!oldpwd)
	{
		if (write(2, "minishell: cd: OLDPWD not set\n", 30) == -1)
			return (NULL);
		return (NULL);
	}
	return (ft_strdup(oldpwd));
}

char	*get_target_path(char **args, char **env)
{
	if (!args[1])
		return (get_home_path(env));
	if (ft_strcmp(args[1], "-") == 0)
		return (get_oldpwd_path(env));
	if (ft_strcmp(args[1], "~") == 0)
		return (get_home_path(env));
	return (ft_strdup(args[1]));
}

int	update_pwd_vars(char ***env, char *old_pwd, char *new_pwd)
{
	int	result;

	result = 0;
	if (set_env_var(env, "OLDPWD", old_pwd) != 0)
		result = 1;
	if (set_env_var(env, "PWD", new_pwd) != 0)
		result = 1;
	return (result);
}

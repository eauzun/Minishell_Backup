#include "../minishell.h"

static void	print_pwd_if_dash(char *path, char *args_1)
{
	if (ft_strcmp(args_1, "-") == 0)
	{
		if (write(1, path, ft_strlen(path)) == -1)
			return ;
		if (write(1, "\n", 1) == -1)
			return ;
	}
}

static int	cd_too_many_args(char **args)
{
	if (args[1] && args[2])
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", 2);
		return (1);
	}
	return (0);
}

static int	cd_chdir_and_getpwd(const char *target, char new_pwd[PATH_MAX])
{
	if (chdir(target) != 0)
	{
		perror("minishell: cd");
		return (1);
	}
	if (!getcwd(new_pwd, PATH_MAX))
		return (1);
	return (0);
}

int	builtin_cd(char **args, char ***env)
{
	char	*target_path;
	char	old_pwd[PATH_MAX];
	char	new_pwd[PATH_MAX];

	if (cd_too_many_args(args))
		return (1);
	if (!getcwd(old_pwd, sizeof(old_pwd)))
		return (1);
	target_path = get_target_path(args, *env);
	if (!target_path)
		return (1);
	if (cd_chdir_and_getpwd(target_path, new_pwd))
	{
		free(target_path);
		return (1);
	}
	if (args[1])
		print_pwd_if_dash(new_pwd, args[1]);
	free(target_path);
	return (update_pwd_vars(env, old_pwd, new_pwd));
}

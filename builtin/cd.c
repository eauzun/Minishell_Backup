/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hialpagu <hialpagu@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 14:06:53 by emuzun            #+#    #+#             */
/*   Updated: 2025/09/02 15:54:03 by hialpagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	cd_too_many_args(char **args)
{
	if (args[1] && args[2])
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", 2);
		return (1);
	}
	return (0);
}

static int	cd_use_pwd_env(char new_pwd[PATH_MAX], char ***env)
{
	char	*pwd_env;

	pwd_env = get_env_value("PWD", *env);
	if (pwd_env && ft_strlen(pwd_env) < PATH_MAX)
	{
		ft_strcpy(new_pwd, pwd_env);
		return (0);
	}
	return (1);
}

static int	cd_handle_dotdot(char new_pwd[PATH_MAX], char ***env)
{
	char	*pwd_env;
	char	*last_slash;
	size_t	len;

	pwd_env = get_env_value("PWD", *env);
	if (!pwd_env)
		return (1);
	last_slash = ft_strrchr(pwd_env, '/');
	if (last_slash && last_slash != pwd_env)
	{
		len = last_slash - pwd_env;
		if (len < PATH_MAX)
		{
			ft_strncpy(new_pwd, pwd_env, len);
			new_pwd[len] = '\0';
			return (0);
		}
	}
	else if (last_slash == pwd_env)
	{
		ft_strcpy(new_pwd, "/");
		return (0);
	}
	return (1);
}

static int	cd_chdir_and_getpwd(const char *target,
		char new_pwd[PATH_MAX], char ***env)
{
	if (chdir(target) != 0)
	{
		perror("minishell: cd");
		return (1);
	}
	if (getcwd(new_pwd, PATH_MAX))
		return (0);
	if (cd_use_pwd_env(new_pwd, env) == 0)
		return (0);
	if (ft_strcmp(target, "..") == 0)
		return (cd_handle_dotdot(new_pwd, env));
	return (1);
}

int	builtin_cd(char **args, char ***env)
{
	char	*target_path;
	char	old_pwd[PATH_MAX];
	char	new_pwd[PATH_MAX];
	char	*current_pwd;

	if (cd_too_many_args(args))
		return (1);
	current_pwd = get_env_value("PWD", *env);
	if (current_pwd && ft_strlen(current_pwd) < PATH_MAX)
		ft_strcpy(old_pwd, current_pwd);
	else if (!getcwd(old_pwd, sizeof(old_pwd)))
		return (1);
	target_path = get_target_path(args, *env);
	if (!target_path)
		return (1);
	if (cd_chdir_and_getpwd(target_path, new_pwd, env))
	{
		free(target_path);
		return (1);
	}
	if (args[1])
		print_pwd_if_dash(new_pwd, args[1]);
	free(target_path);
	return (update_pwd_vars(env, old_pwd, new_pwd));
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor4.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hialpagu <hialpagu@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 15:55:33 by hialpagu          #+#    #+#             */
/*   Updated: 2025/09/02 15:55:34 by hialpagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	command_not_found(char *cmd)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd, 2);
	ft_putstr_fd(": command not found\n", 2);
	return (127);
}

static char	*command_path(t_command *cmd, char ***env, int *status)
{
	char	*path;

	if (ft_strchr(cmd->args[0], '/'))
	{
		*status = check_command_path(cmd->args[0]);
		if (*status != 0)
			return (NULL);
		return (ft_strdup(cmd->args[0]));
	}
	path = find_in_path(cmd->args[0], *env);
	if (!path)
	{
		*status = command_not_found(cmd->args[0]);
		return (NULL);
	}
	*status = 0;
	return (path);
}

static void	execve_command(char *path, t_command *cmd, char **env)
{
	execve(path, cmd->args, env);
	if (cmd->args[0] && (cmd->args[0][0] == '/'
		|| (cmd->args[0][0] == '.' && cmd->args[0][1] == '/')))
	{
		free(path);
		exit(0);
	}
	perror("execve");
	exit(126);
}

int	run_external_command(t_command *cmd, char ***env)
{
	pid_t	pid;
	int		status;
	char	*path;

	path = command_path(cmd, env, &status);
	if (!path)
		return (status);
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		free(path);
		return (1);
	}
	if (pid == 0)
		execve_command(path, cmd, *env);
	waitpid(pid, &status, 0);
	free(path);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (1);
}

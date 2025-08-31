#include "../minishell.h"

static void	child_run_builtin(t_command *cmd, t_pipe_info *info)
{
	int	status;

	status = exec_builtin_or_parent(cmd, info->env);
	cleanup_child(cmd, info);
	exit(status);
}

static void	child_run_external(t_command *cmd, t_pipe_info *info)
{
	char	*path;

	if (ft_strchr(cmd->args[0], '/'))
		path = ft_strdup(cmd->args[0]);
	else
		path = find_in_path(cmd->args[0], *(info->env));
	if (!path)
	{
		ft_putstr_fd("minishell: command not found: ", STDERR_FILENO);
		ft_putstr_fd(cmd->args[0], STDERR_FILENO);
		ft_putstr_fd("\n", STDERR_FILENO);
		cleanup_child(cmd, info);
		exit(127);
	}
	execve(path, cmd->args, *(info->env));
	perror("execve");
	free(path);
	cleanup_child(cmd, info);
	exit(126);
}

void	child_run(t_command *cmd, t_pipe_info *info)
{
	if (apply_redirs(cmd, info->env) != 0)
	{
		cleanup_child(cmd, info);
		exit(1);
	}
	if (is_builtin_command(cmd))
		child_run_builtin(cmd, info);
	else
		child_run_external(cmd, info);
}

void	child_dup2(int oldfd, int newfd, t_command *cmd, t_pipe_info *info)
{
	if (dup2(oldfd, newfd) == -1)
	{
		perror("dup2");
		cleanup_child(cmd, info);
		exit(1);
	}
}
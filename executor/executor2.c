#include "../minishell.h"

int	setup_command_fds(t_command *cmd, char ***env, int *save_fds)
{
	int	heredoc_status;

	save_fds[2] = dup(STDIN_FILENO);
	if (cmd->heredocs)
	{
		heredoc_status = process_heredocs(cmd, *env);
		if (heredoc_status)
		{
			dup2(save_fds[2], STDIN_FILENO);
			close(save_fds[2]);
			return (heredoc_status);
		}
	}
	save_fds[0] = dup(STDIN_FILENO);
	save_fds[1] = dup(STDOUT_FILENO);
	if (apply_redirs(cmd, env) != 0 || cmd->redir_error)
	{
		dup2(save_fds[0], STDIN_FILENO);
		dup2(save_fds[1], STDOUT_FILENO);
		close(save_fds[0]);
		close(save_fds[1]);
		close(save_fds[2]);
		return (g_exit_code(1));
	}
	return (0);
}
int	is_builtin_command(t_command *cmd)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	return (ft_strcmp(cmd->args[0], "cd") == 0
		|| ft_strcmp(cmd->args[0], "echo") == 0
		|| ft_strcmp(cmd->args[0], "exit") == 0
		|| ft_strcmp(cmd->args[0], "pwd") == 0
		|| ft_strcmp(cmd->args[0], "env") == 0
		|| ft_strcmp(cmd->args[0], "export") == 0
		|| ft_strcmp(cmd->args[0], "unset") == 0);
}

static int	exec_builtin_parent(t_command *cmd, char ***env)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	if (ft_strcmp(cmd->args[0], "cd") == 0)
		return (builtin_cd(cmd->args, env));
	if (ft_strcmp(cmd->args[0], "exit") == 0)
		return (builtin_exit(cmd->args));
	if (ft_strcmp(cmd->args[0], "export") == 0)
		return (builtin_export(cmd->args, env));
	if (ft_strcmp(cmd->args[0], "unset") == 0)
		return (builtin_unset(cmd->args, env));
	return (1);
}

static int	exec_builtin(t_command *cmd, char ***env)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	if (ft_strcmp(cmd->args[0], "echo") == 0)
		return (builtin_echo(cmd->args));
	if (ft_strcmp(cmd->args[0], "pwd") == 0)
		return (builtin_pwd());
	if (ft_strcmp(cmd->args[0], "env") == 0)
		return (builtin_env(*env));
	return (1);
}

int	exec_builtin_or_parent(t_command *cmd, char ***env)
{
	int	status;

	status = 0;
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	if (ft_strcmp(cmd->args[0], "cd") == 0
		|| ft_strcmp(cmd->args[0], "exit") == 0
		|| ft_strcmp(cmd->args[0], "export") == 0
		|| ft_strcmp(cmd->args[0], "unset") == 0)
		status = exec_builtin_parent(cmd, env);
	else
		status = exec_builtin(cmd, env);
	g_exit_code(status);
	return (status);
}

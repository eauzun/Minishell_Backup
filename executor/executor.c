#include "../minishell.h"

int	set_wait_status(int status)
{
	if (WIFEXITED(status))
		g_exit_code(WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		g_exit_code(128 + WTERMSIG(status));
	return (g_exit_code(-1));
}

int	run_external_command(t_command *cmd, char ***env)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		g_exit_code(1);
		return (1);
	}
	if (pid == 0)
	{
		set_child_signals();
		exit(run_child(cmd, env));
	}
	if (waitpid(pid, &status, 0) == -1)
	{
		perror("waitpid");
		g_exit_code(1);
		return (1);
	}
	return (set_wait_status(status));
}

int	exec_builtin_or_parent(t_command *cmd, char ***env)
{
	int	exit_status;

	if (!is_builtin(cmd->args[0]))
		return (-1);
	if (cmd->redirs == NULL && !cmd->heredoc)
		exit_status = execute_builtin(cmd, env);
	else
		exit_status = execute_builtin_parent(cmd, env);
	g_exit_code(exit_status);
	return (exit_status);
}

int	execute_single_command(t_command *cmd, char ***env)
{
	int	exit_status;

	if (!cmd || !cmd->args || !cmd->args[0])
	{
		g_exit_code(0);
		return (0);
	}
	exit_status = exec_builtin_or_parent(cmd, env);
	if (exit_status != -1)
		return (exit_status);
	return (run_external_command(cmd, env));
}

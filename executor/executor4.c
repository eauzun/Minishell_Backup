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

static int	handle_parent_process(pid_t pid, char *path)
{
	int	status;

	waitpid(pid, &status, 0);
	free(path);
	restore_signals();
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

int	run_external_command(t_command *cmd, char ***env)
{
	pid_t	pid;
	int		status;
	char	*path;

	path = command_path(cmd, env, &status);
	if (!path)
		return (status);
	set_execution_signals();
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		free(path);
		restore_signals();
		return (1);
	}
	if (pid == 0)
		handle_child_process(path, cmd, *env);
	return (handle_parent_process(pid, path));
}

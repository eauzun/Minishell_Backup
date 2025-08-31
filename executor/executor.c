#include "../minishell.h"

int	execute_single_command(t_command *cmd, char ***env)
{
	int	status;
	int	save_in;
	int	save_out;

	status = 0;
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	save_in = dup(STDIN_FILENO);
	save_out = dup(STDOUT_FILENO);
	if (apply_redirs(cmd, env) != 0)
	{
		restore_stdio(save_in, save_out);
		return (1);
	}
	if (is_builtin_command(cmd))
		status = exec_builtin_or_parent(cmd, env);
	else
		status = run_external_command(cmd, env);
	restore_stdio(save_in, save_out);
	g_exit_code(status);
	return (status);
}

int	run_external_command(t_command *cmd, char ***env)
{
	pid_t	pid;
	int		status;
	char	*path;

	if (ft_strchr(cmd->args[0], '/'))
		path = ft_strdup(cmd->args[0]);
	else
		path = find_in_path(cmd->args[0], *env);
	if (!path)
		return (command_not_found(cmd->args[0]));
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

void	execve_command(char *path, t_command *cmd, char **env)
{
	execve(path, cmd->args, env);
	perror("execve");
	exit(126);
}

int	command_not_found(char *cmd)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd, 2);
	ft_putstr_fd(": command not found\n", 2);
	return (127);
}

void	restore_stdio(int in_fd, int out_fd)
{
	dup2(in_fd, STDIN_FILENO);
	dup2(out_fd, STDOUT_FILENO);
	close(in_fd);
	close(out_fd);
}

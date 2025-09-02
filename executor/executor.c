#include "../minishell.h"

static int	handle_dup2(int fd, t_redir *cur, t_command *cmd)
{
	if (cur->type == R_IN)
	{
		if (dup2(fd, STDIN_FILENO) == -1)
		{
			perror("dup2");
			close(fd);
			cmd->redir_error = 1;
			g_exit_code(1);
			return (1);
		}
	}
	else
	{
		if (dup2(fd, STDOUT_FILENO) == -1)
		{
			perror("dup2");
			close(fd);
			cmd->redir_error = 1;
			g_exit_code(1);
			return (1);
		}
	}
	close(fd);
	return (0);
}

static int	apply_single_redir(t_redir *cur, t_command *cmd)
{
	int	fd;

	fd = -1;
	if (cur->type == R_IN)
		fd = open(cur->file, O_RDONLY);
	else if (cur->type == R_OUT)
		fd = open(cur->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (cur->type == R_APPEND)
		fd = open(cur->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		perror(cur->file);
		cmd->redir_error = 1;
		g_exit_code(1);
		return (1);
	}
	return (handle_dup2(fd, cur, cmd));
}

int	apply_redirs(t_command *cmd, char ***env)
{
	t_redir	*cur;

	(void)env;
	cur = cmd->redirs;
	while (cur)
	{
		if (apply_single_redir(cur, cmd) != 0)
			return (1);
		cur = cur->next;
	}
	return (0);
}

static void	restore_fds(int *save_fds)
{
	dup2(save_fds[0], STDIN_FILENO);
	dup2(save_fds[1], STDOUT_FILENO);
	close(save_fds[0]);
	close(save_fds[1]);
	dup2(save_fds[2], STDIN_FILENO);
	close(save_fds[2]);
}

int	execute_single_command(t_command *cmd, char ***env)
{
	int	status;
	int	save_fds[3];
	int	setup_result;

	status = 0;
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	setup_result = setup_command_fds(cmd, env, save_fds);
	if (setup_result)
		return (setup_result);
	if (is_builtin_command(cmd))
		status = exec_builtin_or_parent(cmd, env);
	else
		status = run_external_command(cmd, env);
	restore_fds(save_fds);
	g_exit_code(status);
	return (status);
}

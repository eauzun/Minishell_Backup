#include "../minishell.h"

static int	open_redir_file(t_redir *cur, t_command *cmd)
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
		return (-1);
	}
	return (fd);
}

static int	dup2_redir(int fd, t_redir *cur, t_command *cmd)
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

	fd = open_redir_file(cur, cmd);
	if (fd == -1)
		return (1);
	return (dup2_redir(fd, cur, cmd));
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

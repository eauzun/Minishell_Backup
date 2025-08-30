#include "../minishell.h"

int	redirect_in(const char *path)
{
	int	fd;

	fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		syntax_error("No such file or directory\n");
		return (1);
	}
	if (dup2(fd, STDIN_FILENO) < 0)
	{
		perror("dup2");
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

int	redirect_out(const char *path, int append)
{
	int	flags;
	int	fd;

	flags = O_CREAT | O_WRONLY;
	if (append)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	fd = open(path, flags, 0644);
	if (fd < 0)
	{
		syntax_error("Permission denied\n");
		return (1);
	}
	if (dup2(fd, STDOUT_FILENO) < 0)
	{
		perror("dup2");
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

int	apply_redirs(t_command *cmd, char ***env)
{
	t_redir	*redir;

	if (cmd->heredoc && exec_heredoc(cmd, *env))
		return (1);
	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == R_IN && redirect_in(redir->file))
			return (1);
		if (redir->type == R_OUT && redirect_out(redir->file, 0))
			return (1);
		if (redir->type == R_APPEND && redirect_out(redir->file, 1))
			return (1);
		redir = redir->next;
	}
	return (0);
}

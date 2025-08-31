#include "../minishell.h"

static int	apply_single_redir(t_redir *cur, t_command *cmd)
{
	int	fd;

	fd = -1;
	if (cur->type == R_IN) // < input redirection
		fd = open(cur->file, O_RDONLY);
	else if (cur->type == R_OUT) // > output redirection
		fd = open(cur->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (cur->type == R_APPEND) // >> append redirection
		fd = open(cur->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		perror(cur->file);
		cmd->redir_error = 1;
		return (1);
	}
	if (cur->type == R_IN)
		dup2(fd, STDIN_FILENO);
	else // R_OUT or R_APPEND
		dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
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
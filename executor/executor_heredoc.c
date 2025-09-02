#include "../minishell.h"

void	heredoc_signal_handler(int sig)
{
	(void)sig;
	write(1, "\n", 1);
	g_exit_code(130);
	rl_replace_line("", 0);
	rl_done = 1;
}

int	process_single_heredoc(t_heredoc *heredoc, char **env)
{
	void	(*old_int)(int);
	int		status;

	if (pipe(heredoc->pipe_fd) == -1)
	{
		perror("pipe");
		return (1);
	}
	old_int = signal(SIGINT, heredoc_signal_handler);
	status = read_heredoc_line(heredoc->delimiter, heredoc->expand_vars,
			env, heredoc->pipe_fd[1]);
	signal(SIGINT, old_int);
	close(heredoc->pipe_fd[1]);
	if (status == 130)
	{
		close(heredoc->pipe_fd[0]);
		return (130);
	}
	return (status);
}

static int	handle_heredoc_error(void)
{
	if (g_exit_code(-1) == 130)
		return (130);
	return (1);
}

static void	connect_last_heredoc(t_heredoc *last)
{
	if (!last)
		return ;
	dup2(last->pipe_fd[0], STDIN_FILENO);
	close(last->pipe_fd[0]);
}

int	process_heredocs(t_command *cmd, char **env)
{
	t_heredoc	*cur;
	t_heredoc	*last;

	if (!cmd->heredocs)
		return (0);
	cur = cmd->heredocs;
	last = NULL;
	while (cur)
	{
		if (process_single_heredoc(cur, env))
			return (handle_heredoc_error());
		if (last)
			close(last->pipe_fd[0]);
		last = cur;
		cur = cur->next;
	}
	connect_last_heredoc(last);
	return (0);
}

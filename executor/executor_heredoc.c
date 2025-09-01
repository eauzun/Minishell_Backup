#include "../minishell.h"

void	heredoc_signal_handler(int sig)
{
	(void)sig;
	write(1, "\n", 1);
	g_exit_code(130);
	rl_replace_line("", 0);
	rl_done = 1; // readline hemen NULL döner
}

static int	read_heredoc_line(char *delimiter, int expand, char **env, int write_fd)
{
	char	*line;
	char	*expanded;
	int		initial_exit_code;

	initial_exit_code = g_exit_code(-1);
	while (1)
	{
		line = readline("> ");
		if (!line) // NULL -> Ctrl-D veya Ctrl-C sonrası rl_done
		{
			write(2, "warning: here-document delimited by end-of-file (wanted `", 58);
			write(2, delimiter, ft_strlen(delimiter));
			write(2, "')\n", 3);
			return (0);
		}
		if (g_exit_code(-1) == 130) // Ctrl-C
				return (130);
		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			g_exit_code(initial_exit_code);
			return (0);
		}
		if (expand)
		{
			expanded = expand_string(line, env);
			free(line);
			if (!expanded)
				return (1);
			line = expanded;
		}
		write(write_fd, line, ft_strlen(line));
		write(write_fd, "\n", 1);
		free(line);
	}
}



static int	setup_heredoc_pipe(t_heredoc *heredoc)
{
	if (pipe(heredoc->pipe_fd) == -1)
	{
		perror("pipe");
		return (1);
	}
	return (0);
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
	// heredoc için Ctrl-C handler
	old_int = signal(SIGINT, heredoc_signal_handler);

	status = read_heredoc_line(heredoc->delimiter, heredoc->expand_vars,
			env, heredoc->pipe_fd[1]);

	// eski handler geri yükle
	signal(SIGINT, old_int);
	close(heredoc->pipe_fd[1]);
	if (status == 130) // Ctrl-C
	{
		close(heredoc->pipe_fd[0]);
		return (130);
	}
	return (status);
}



int	process_heredocs(t_command *cmd, char **env)
{
	t_heredoc	*cur;
	t_heredoc	*last_valid;

	if (!cmd->heredocs)
		return (0);
	last_valid = NULL;
	cur = cmd->heredocs;
	while (cur)
	{
		if (process_single_heredoc(cur, env))
		{
			if (g_exit_code(-1) == 130)
				return (130);
			return (1);
		}
		if (last_valid)
			close(last_valid->pipe_fd[0]);
		last_valid = cur;
		cur = cur->next;
	}
	if (last_valid)
	{
		dup2(last_valid->pipe_fd[0], STDIN_FILENO);
		close(last_valid->pipe_fd[0]);
	}
	return (0);
}



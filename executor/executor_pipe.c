#include "../minishell.h"

void	setup_pipe_ends(int i, int n, int (*pipes)[2], t_command *c)
{
	if (i > 0 && !c->infile && !c->heredoc_prev)
		dup2(pipes[i - 1][0], STDIN_FILENO);
	if (i < n - 1 && !c->outfile)
		dup2(pipes[i][1], STDOUT_FILENO);
	close_all(pipes, n - 1);
}

void	execute_command(t_command *c, char ***env)
{
	char	*path;

	if (!c->args || !c->args[0])
		exit(0);
	if (is_builtin(c->args[0]))
		exit(execute_builtin(c, env));
	if (ft_strchr(c->args[0], '/'))
		exec_or_die(c->args[0], c, *env);
	else
	{
		path = find_in_path(c->args[0], *env);
		exec_or_die(path, c, *env);
	}
}

void	child_process(t_command *c, t_pipe_info *info, char ***env)
{
	set_child_signals();
	if (apply_redirs(c, env))
		exit(1);
	setup_pipe_ends(info->index, info->total, info->pipes, c);
	execute_command(c, env);
}

int	wait_all_children(pid_t *pids, int n)
{
	int	i;
	int	status;
	int	last_status;

	i = 0;
	last_status = 0;
	while (i < n)
	{
		if (waitpid(pids[i], &status, 0) == -1)
			perror("waitpid");
		if (i == n - 1)
		{
			if (WIFEXITED(status))
				last_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				last_status = 128 + WTERMSIG(status);
			else
				last_status = 1;
		}
		i++;
	}
	return (last_status);
}

#include "../minishell.h"

int	create_pipes(int (*pipes)[2], int total)
{
	int	i;

	i = 0;
	while (i < total - 1)
	{
		if (pipe(pipes[i]) == -1)
		{
			perror("pipe");
			return (1);
		}
		i++;
	}
	return (0);
}

void	spawn_all_children(t_command *cmds, pid_t *pids, t_pipe_info *info,
	char ***env)
{
	int			i;
	t_command	*c;
	pid_t		pid;

	i = 0;
	c = cmds;
	while (i < info->total)
	{
		info->index = i;
		pid = fork();
		if (pid == 0)
			child_process(c, info, env);
		pids[i] = pid;
		c = c->next;
		i++;
	}
}

int	run_pipeline(t_command *cmds, char ***env)
{
	int			n;
	int			pipes[cmd_count(cmds) - 1][2];
	pid_t		pids[cmd_count(cmds)];
	t_pipe_info	info;

	n = cmd_count(cmds);
	info.total = n;
	info.pipes = pipes;
	if (create_pipes(info.pipes, info.total))
		return (1);
	spawn_all_children(cmds, pids, &info, env);
	close_all(info.pipes, n - 1);
	return (wait_all_children(pids, n));
}

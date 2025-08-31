#include "../minishell.h"

static void	child_process(t_command *cmd, int index, t_pipe_info *info)
{
	set_child_signals();
	if (index == 0 && info->n > 1) // ilk komut
		child_dup2(info->pipes[1], STDOUT_FILENO, cmd, info);
	else if (index == info->n - 1 && info->n > 1) // son komut
		child_dup2(info->pipes[(index - 1) * 2], STDIN_FILENO, cmd, info);
	else if (index > 0 && index < info->n - 1) // ara komutlar
	{
		child_dup2(info->pipes[(index - 1) * 2], STDIN_FILENO, cmd, info);
		child_dup2(info->pipes[index * 2 + 1], STDOUT_FILENO, cmd, info);
	}
	close_all_pipes(info->pipes, info->n - 1);
	child_run(cmd, info);
}

static int	work_child_work(t_command *cmds, t_pipe_info *info)
{
	t_command	*cur;
	int			i;

	cur = cmds;
	i = 0;
	while (i < info->n)
	{
		info->pids[i] = fork();
		if (info->pids[i] == -1)
			return (1);
		if (info->pids[i] == 0)
			child_process(cur, i, info);
		cur = cur->next;
		i++;
	}
	return (0);
}

static int	wait_all_children(pid_t *pids, int count)
{
	int	status;
	int	i;
	int	exit_code;

	exit_code = 0;
	i = 0;
	while (i < count)
	{
		if (waitpid(pids[i], &status, 0) == -1)
			perror("waitpid");
		else if (WIFEXITED(status))
			exit_code = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			exit_code = 128 + WTERMSIG(status);
		i++;
	}
	return (exit_code);
}

int	run_pipeline(t_command *cmds, char ***env)
{
	t_pipe_info	info;
	int			status;

	info.n = cmd_count(cmds);
	info.pipes = setup_pipes(info.n - 1);
	if (!info.pipes && info.n > 1)
		return (1);
	info.pids = malloc(sizeof(pid_t) * info.n);
	if (!info.pids)
	{
		free(info.pipes);
		return (1);
	}
	info.env = env;
	if (work_child_work(cmds, &info))
	{
		free(info.pipes);
		free(info.pids);
		return (1);
	}
	close_all_pipes(info.pipes, info.n - 1);
	status = wait_all_children(info.pids, info.n);
	free(info.pipes);
	free(info.pids);
	return (status);
}

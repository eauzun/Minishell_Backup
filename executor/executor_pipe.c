#include "../minishell.h"

static int	cmd_count(t_command *cmds)
{
	int	count;

	count = 0;
	while (cmds)
	{
		count++;
		cmds = cmds->next;
	}
	return (count);
}

static int	*setup_pipes(int count)
{
	int	*pipes;
	int	i;

	if (count <= 0)
		return (NULL);
	pipes = malloc(sizeof(int) * (count * 2));
	if (!pipes)
		return (NULL);
	i = 0;
	while (i < count)
	{
		if (pipe(&pipes[i * 2]) == -1)
		{
			while (i > 0)
			{
				close(pipes[(i - 1) * 2]);
				close(pipes[(i - 1) * 2 + 1]);
				i--;
			}
			free(pipes);
			return (NULL);
		}
		i++;
	}
	return (pipes);
}

static void	cleanup_child(t_command *cmd, t_pipe_info *info)
{
	int	i;

	i = 0;
	if (info->pipes)
		free(info->pipes);
	if (info->pids)
		free(info->pids);
	if (cmd && cmd->args)
	{
		i = 0;
		while (cmd->args[i])
		{
			free(cmd->args[i]);
			i++;
		}
		free(cmd->args);
		cmd->args = NULL;
	}
}

static void	close_all_pipes(int *pipes, int count)
{
	int	i;

	if (!pipes || count <= 0)
		return ;
	i = 0;
	while (i < count)
	{
		close(pipes[i * 2]); // read
		close(pipes[i * 2 + 1]); // write
		i++;
	}
}

static void	child_run_builtin(t_command *cmd, t_pipe_info *info)
{
	int	status;

	status = exec_builtin_or_parent(cmd, info->env);
	cleanup_child(cmd, info);
	exit(status);
}

static void	child_run_external(t_command *cmd, t_pipe_info *info)
{
	char	*path;

	path = find_in_path(cmd->args[0], *(info->env));
	if (!path)
	{
		ft_putstr_fd("minishell: command not found: ", STDERR_FILENO);
		ft_putstr_fd(cmd->args[0], STDERR_FILENO);
		ft_putstr_fd("\n", STDERR_FILENO);
		cleanup_child(cmd, info);
		exit(127);
	}
	execve(path, cmd->args, *(info->env));
	perror("execve");
	free(path);
	cleanup_child(cmd, info);
	exit(126);
}
static void	child_run(t_command *cmd, t_pipe_info *info)
{
	if (apply_redirs(cmd, info->env) != 0)
	{
		cleanup_child(cmd, info);
		exit(1);
	}
	if (is_builtin_command(cmd))
		child_run_builtin(cmd, info);
	else
		child_run_external(cmd, info);
}

static void	child_dup2(int oldfd, int newfd, t_command *cmd, t_pipe_info *info)
{
	if (dup2(oldfd, newfd) == -1)
	{
		perror("dup2");
		cleanup_child(cmd, info);
		exit(1);
	}
}

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
		if (info->pids[i] == 0) // çocuğa görev atanıyor
			child_process(cur, i, info); // tek kritik nokta → child_process içinde başarısızlık durumunda mutlaka exit(1) çağırman gerekiyor.
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

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

// static void	cleanup_child(t_command *cmd, t_token *tokens,
// 									t_pipe_info *info, int exit_code)
// {
// 	if (info->pipes)
// 		free(info->pipes);
// 	if (info->pids)
// 		free(info->pids);
// 	if (cmd)
// 		free_commands(cmd); 
// 	if (tokens)
// 		free_token(tokens); 
// 	if (info->env && *(info->env))
// 	{
// 		free_env_data(*(info->env));
// 		*(info->env) = NULL;
// 	}
// 	rl_clear_history();
// 	exit(exit_code);
// }


// static void	close_all_pipes(int *pipes, int count)
// {
// 	int	i;

// 	if (!pipes || count <= 0)
// 		return ;
// 	i = 0;
// 	while (i < count)
// 	{
// 		close(pipes[i * 2]);
// 		close(pipes[i * 2 + 1]); 
// 		i++;
// 	}
// }
static int	child_builtin_exec(t_command *cmd, char ***env)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	if (ft_strcmp(cmd->args[0], "echo") == 0)
		return (builtin_echo(cmd->args));
	if (ft_strcmp(cmd->args[0], "pwd") == 0)
		return (builtin_pwd());
	if (ft_strcmp(cmd->args[0], "env") == 0)
		return (builtin_env(*env));
	return (0);
}
static void	child_run_builtin(t_command *cmd, t_pipe_info *info, t_token *tokens)
{
	int	status;

	status = child_builtin_exec(cmd, info->env);
	cleanup_child(cmd, tokens, info, status);
}

static void	child_run_external(t_command *cmd, t_pipe_info *info, t_token *tokens)
{
	char	*path;

	path = find_in_path(cmd->args[0], *(info->env));
	if (!path)
	{
		ft_putstr_fd("minishell: command not found: ", STDERR_FILENO);
		ft_putstr_fd(cmd->args[0], STDERR_FILENO);
		ft_putstr_fd("\n", STDERR_FILENO);
		cleanup_child(cmd, tokens, info, 127);
	}
	if (access(path, X_OK) != 0)
	{
    	perror(cmd->args[0]);
		free(path);
    	cleanup_child(cmd, tokens, info, 126);
	}
	execve(path, cmd->args, *(info->env));
	perror("execve");
	free(path);
	cleanup_child(cmd, tokens, info, 126);
}

static void	child_run(t_command *cmd, t_pipe_info *info, t_token *tokens)
{
	if (apply_redirs(cmd, info->env) != 0)
		cleanup_child(cmd, NULL, info, 1);
	if (is_builtin_command(cmd))
		child_run_builtin(cmd, info, tokens);
	else
		child_run_external(cmd, info, tokens);
}

static void	child_dup2(int oldfd, int newfd, t_command *cmd, t_pipe_info *info, t_token *tokens)
{
	if (dup2(oldfd, newfd) == -1)
	{
		perror("dup2");
		cleanup_child(cmd, tokens, info, 1);
	}
}

static void	child_process(t_command *cmd, int index, t_pipe_info *info, t_token *tokens)
{
	set_child_signals();
	if (index == 0 && info->n > 1) 
		child_dup2(info->pipes[1], STDOUT_FILENO, cmd, info, tokens);
	else if (index == info->n - 1 && info->n > 1) 
		child_dup2(info->pipes[(index - 1) * 2], STDIN_FILENO, cmd, info, tokens);
	else if (index > 0 && index < info->n - 1) 
	{
		child_dup2(info->pipes[(index - 1) * 2], STDIN_FILENO, cmd, info, tokens);
		child_dup2(info->pipes[index * 2 + 1], STDOUT_FILENO, cmd, info, tokens);
	}
	close_all_pipes(info->pipes, info->n - 1);
	child_run(cmd, info, tokens);
}

static int	work_child_work(t_command *cmds, t_pipe_info *info, t_token *tokens)
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
			child_process(cur, i, info, tokens); 
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


static int	preprocess_pipeline_heredocs(t_command *cmds, char **env)
{
	t_command	*cur;
	int			status;
	int			backup_stdin;

	cur = cmds;
	backup_stdin = dup(STDIN_FILENO); 
	while (cur)
	{
		if (cur->heredocs)
		{
			status = process_heredocs(cur, env);
			if (status)
			{
				close(backup_stdin);
				return (status);
			}
		}
		cur = cur->next;
	}
	close(backup_stdin);
	return (0);
}

int	run_pipeline(t_command *cmds, char ***env, t_token *tokens)
{
	t_pipe_info	info;
	int			status;
	int			heredoc_status;

	heredoc_status = preprocess_pipeline_heredocs(cmds, *env);
	if (heredoc_status)
		return (heredoc_status);

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
	if (work_child_work(cmds, &info, tokens))
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

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hialpagu <hialpagu@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 15:55:15 by hialpagu          #+#    #+#             */
/*   Updated: 2025/09/02 15:55:15 by hialpagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

static int	work_child_work(t_command *cmds, t_pipe_info *info,
						t_token *tokens)
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

static int	init_pipe_info(t_pipe_info *info, t_command *cmds)
{
	info->n = cmd_count(cmds);
	info->pipes = setup_pipes(info->n - 1);
	if (!info->pipes && info->n > 1)
		return (1);
	info->pids = malloc(sizeof(pid_t) * info->n);
	if (!info->pids)
	{
		free(info->pipes);
		return (1);
	}
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
	if (init_pipe_info(&info, cmds))
		return (1);
	info.env = env;
	if (work_child_work(cmds, &info, tokens))
	{
		cleanup_pipe_info(&info);
		return (1);
	}
	close_all_pipes(info.pipes, info.n - 1);
	status = wait_all_children(info.pids, info.n);
	cleanup_pipe_info(&info);
	return (status);
}

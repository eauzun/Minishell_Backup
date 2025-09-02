/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe_free_utils.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emuzun <emuzun@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 16:03:28 by emuzun            #+#    #+#             */
/*   Updated: 2025/09/02 16:03:29 by emuzun           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	cleanup_child(t_command *cmd, t_token *tokens,
									t_pipe_info *info, int exit_code)
{
	if (info->pipes)
		free(info->pipes);
	if (info->pids)
		free(info->pids);
	if (cmd)
		free_commands(cmd);
	if (tokens)
		free_token(tokens);
	if (info->env && *(info->env))
	{
		free_env_data(*(info->env));
		*(info->env) = NULL;
	}
	rl_clear_history();
	exit(exit_code);
}

void	close_all_pipes(int *pipes, int count)
{
	int	i;

	if (!pipes || count <= 0)
		return ;
	i = 0;
	while (i < count)
	{
		close(pipes[i * 2]);
		close(pipes[i * 2 + 1]);
		i++;
	}
}

void	cleanup_pipe_info(t_pipe_info *info)
{
	if (info->pipes)
		free(info->pipes);
	if (info->pids)
		free(info->pids);
}

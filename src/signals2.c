/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hialpagu <hialpagu@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 00:08:26 by hialpagu          #+#    #+#             */
/*   Updated: 2025/09/03 00:08:26 by hialpagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	execve_command(char *path, t_command *cmd, char **env)
{
	execve(path, cmd->args, env);
	if (cmd->args[0] && (cmd->args[0][0] == '/'
		|| (cmd->args[0][0] == '.' && cmd->args[0][1] == '/')))
	{
		free(path);
		exit(0);
	}
	perror("execve");
	exit(126);
}

void	handle_child_process(char *path, t_command *cmd, char **env)
{
	set_child_signals();
	execve_command(path, cmd, env);
}

void	set_execution_signals(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

void	restore_signals(void)
{
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, SIG_IGN);
}

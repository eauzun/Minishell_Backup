#include "../minishell.h"

static void	setup_first_child_pipes(t_command *cmd, t_pipe_info *info,
							t_token *tokens)
{
	if (dup2(info->pipes[1], STDOUT_FILENO) == -1)
	{
		perror("dup2");
		cleanup_child(cmd, tokens, info, 1);
	}
}

static void	setup_last_child_pipes(t_command *cmd, int index,
							t_pipe_info *info, t_token *tokens)
{
	if (dup2(info->pipes[(index - 1) * 2], STDIN_FILENO) == -1)
	{
		perror("dup2");
		cleanup_child(cmd, tokens, info, 1);
	}
}

static void	setup_middle_child_pipes(t_command *cmd, int index,
							t_pipe_info *info, t_token *tokens)
{
	if (dup2(info->pipes[(index - 1) * 2], STDIN_FILENO) == -1)
	{
		perror("dup2");
		cleanup_child(cmd, tokens, info, 1);
	}
	if (dup2(info->pipes[index * 2 + 1], STDOUT_FILENO) == -1)
	{
		perror("dup2");
		cleanup_child(cmd, tokens, info, 1);
	}
}

void	child_process(t_command *cmd, int index, t_pipe_info *info,
					t_token *tokens)
{
	set_child_signals();
	if (index == 0 && info->n > 1)
		setup_first_child_pipes(cmd, info, tokens);
	else if (index == info->n - 1 && info->n > 1)
		setup_last_child_pipes(cmd, index, info, tokens);
	else if (index > 0 && index < info->n - 1)
		setup_middle_child_pipes(cmd, index, info, tokens);
	close_all_pipes(info->pipes, info->n - 1);
	child_run(cmd, info, tokens);
}
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

int	preprocess_pipeline_heredocs(t_command *cmds, char **env)
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

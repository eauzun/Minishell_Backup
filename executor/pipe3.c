#include "../minishell.h"

int	cmd_count(t_command *cmds)
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

int	*setup_pipes(int count)
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

void	cleanup_child(t_command *cmd, t_pipe_info *info)
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

void	close_all_pipes(int *pipes, int count)
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
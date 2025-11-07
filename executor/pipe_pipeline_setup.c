#include "../minishell.h"

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

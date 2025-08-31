#include "../minishell.h"
/*
void child_clean_exit(int status)
{
	rl_clear_history();
	exit(status & 0xFF);
}

void child_clean_exit_env(int status, char **env)
{
	rl_clear_history();
	if (env)
		free_env_data(env);
	exit(status & 0xFF);
}

void child_pre_exec_sanitize(void)
{
	rl_clear_history();
}

void child_clean_exit_env_cmds(int status, char **env, t_command *head)
{
	rl_clear_history();
	if (head)
		free_commands(head);
	if (env)
		free_env_data(env);
	exit(status & 0xFF);
}

*/

// pipes ve pids serbest bırakma
void	free_pipes_and_pids(int **pipes, int n, pid_t *pids)
{
	int	i;

	if (pipes)
	{
		i = 0;
		while (i < n - 1)
		{
			if (pipes[i])
				free(pipes[i]);
			i++;
		}
		free(pipes);
	}
	if (pids)
		free(pids);
}

// env serbest bırakma
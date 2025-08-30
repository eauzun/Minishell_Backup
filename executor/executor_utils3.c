#include "../minishell.h"

void	handle_exec_errors(char *prog, t_command *cmd)
{
	char	**av;

	if (is_reg(prog))
	{
		av = make_sh_argv(cmd, prog);
		if (av)
		{
			execve("/bin/sh", av, NULL);
			free(av);
		}
	}
	free(prog);
	print_error("minishell: ", cmd->args[0], ": execution error\n");
	exit(127);
}

void	exec_or_die(char *prog, t_command *cmd, char **env)
{
	if (!prog)
	{
		print_error("minishell: ", cmd->args[0], ": command not found\n");
		exit(127);
	}
	if (is_dir(prog))
	{
		print_error("minishell: ", cmd->args[0], ": is a directory\n");
		exit(126);
	}
	if (access(prog, F_OK) == 0 && access(prog, X_OK) != 0)
	{
		print_error("minishell: ", cmd->args[0], ": permission denied\n");
		exit(126);
	}
	execve(prog, cmd->args, env);
	handle_exec_errors(prog, cmd);
}

int	cmd_count(t_command *cmd)
{
	int	i;

	i = 0;
	while (cmd)
	{
		i++;
		cmd = cmd->next;
	}
	return (i);
}

void	close_all(int (*pipes)[2], int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		i++;
	}
}

#include "../minishell.h"

static void	restore_fds(int *save_fds)
{
	dup2(save_fds[0], STDIN_FILENO);
	dup2(save_fds[1], STDOUT_FILENO);
	close(save_fds[0]);
	close(save_fds[1]);
	dup2(save_fds[2], STDIN_FILENO);
	close(save_fds[2]);
}

int	execute_single_command(t_command *cmd, char ***env)
{
	int	status;
	int	save_fds[3];
	int	setup_result;

	status = 0;
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	setup_result = setup_command_fds(cmd, env, save_fds);
	if (setup_result)
		return (setup_result);
	if (is_builtin_command(cmd))
		status = exec_builtin_or_parent(cmd, env);
	else
		status = run_external_command(cmd, env);
	restore_fds(save_fds);
	g_exit_code(status);
	return (status);
}

#include "../minishell.h"

static int	child_builtin_exec(t_command *cmd, char ***env)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	if (ft_strcmp(cmd->args[0], "echo") == 0)
		return (builtin_echo(cmd->args));
	if (ft_strcmp(cmd->args[0], "pwd") == 0)
		return (builtin_pwd());
	if (ft_strcmp(cmd->args[0], "env") == 0)
		return (builtin_env(*env));
	return (0);
}

void	child_run_builtin(t_command *cmd, t_pipe_info *info, t_token *tokens)
{
	int	status;

	status = child_builtin_exec(cmd, info->env);
	cleanup_child(cmd, tokens, info, status);
}

void	child_run_external(t_command *cmd, t_pipe_info *info, t_token *tokens)
{
	char	*path;

	path = find_in_path(cmd->args[0], *(info->env));
	if (!path)
	{
		ft_putstr_fd("minishell: command not found: ", STDERR_FILENO);
		ft_putstr_fd(cmd->args[0], STDERR_FILENO);
		ft_putstr_fd("\n", STDERR_FILENO);
		cleanup_child(cmd, tokens, info, 127);
	}
	if (access(path, X_OK) != 0)
	{
		perror(cmd->args[0]);
		free(path);
		cleanup_child(cmd, tokens, info, 126);
	}
	execve(path, cmd->args, *(info->env));
	perror("execve");
	free(path);
	cleanup_child(cmd, tokens, info, 126);
}

void	child_run(t_command *cmd, t_pipe_info *info, t_token *tokens)
{
	if (apply_redirs(cmd, info->env) != 0)
		cleanup_child(cmd, NULL, info, 1);
	if (is_builtin_command(cmd))
		child_run_builtin(cmd, info, tokens);
	else
		child_run_external(cmd, info, tokens);
}
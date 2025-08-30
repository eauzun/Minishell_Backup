#include "../minishell.h"

void	restore_stdio(int in_fd, int out_fd)
{
	dup2(in_fd, STDIN_FILENO);
	dup2(out_fd, STDOUT_FILENO);
	close(in_fd);
	close(out_fd);
}

int	run_child(t_command *cmd, char ***env)
{
	char	*prog;

	set_child_signals();
	if (apply_redirs(cmd, env))
		exit(1);
	if (!cmd->args || !cmd->args[0])
		exit(0);
	if (is_builtin(cmd->args[0]))
		exit(execute_builtin(cmd, env));
	if (contains_slash(cmd->args[0]))
		exec_or_die(cmd->args[0], cmd, *env);
	prog = find_in_path(cmd->args[0], *env);
	exec_or_die(prog, cmd, *env);
	return (0);
}

int	execute_builtin_parent(t_command *cmd, char ***env)
{
	int	save_in;
	int	save_out;
	int	exit_status;

	save_in = dup(STDIN_FILENO);
	save_out = dup(STDOUT_FILENO);
	if (save_in < 0 || save_out < 0)
		return (1);
	if (cmd->redir_error)
	{
		syntax_error("No such file or directory\n");
		restore_stdio(save_in, save_out);
		return (1);
	}
	if (apply_redirs(cmd, env))
	{
		restore_stdio(save_in, save_out);
		return (1);
	}
	exit_status = execute_builtin(cmd, env);
	restore_stdio(save_in, save_out);
	return (exit_status);
}

void	ft_putstr_fd(const char *s, int fd)
{
	int	i;

	i = 0;
	while (s[i] != '\0')
	{
		write(fd, &s[i], 1);
		i++;
	}
}

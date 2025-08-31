#include "../minishell.h"

static int	apply_single_redir(t_redir *cur, t_command *cmd)
{
	int	fd;

	fd = -1;
	if (cur->type == R_IN) // < input redirection
		fd = open(cur->file, O_RDONLY);
	else if (cur->type == R_OUT) // > output redirection
		fd = open(cur->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (cur->type == R_APPEND) // >> append redirection
		fd = open(cur->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		perror(cur->file);
		cmd->redir_error = 1;
		return (1);
	}
	if (cur->type == R_IN)
		dup2(fd, STDIN_FILENO);
	else // R_OUT or R_APPEND
		dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

int	apply_redirs(t_command *cmd, char ***env)
{
	t_redir	*cur;

	(void)env;
	cur = cmd->redirs;
	while (cur)
	{
		if (apply_single_redir(cur, cmd) != 0)
			return (1);
		cur = cur->next;
	}
	return (0);
}

int	is_builtin_command(t_command *cmd)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	return (ft_strcmp(cmd->args[0], "cd") == 0
		|| ft_strcmp(cmd->args[0], "echo") == 0
		|| ft_strcmp(cmd->args[0], "exit") == 0
		|| ft_strcmp(cmd->args[0], "pwd") == 0
		|| ft_strcmp(cmd->args[0], "env") == 0
		|| ft_strcmp(cmd->args[0], "export") == 0
		|| ft_strcmp(cmd->args[0], "unset") == 0);
}
static int	exec_builtin_parent(t_command *cmd, char ***env)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	if (ft_strcmp(cmd->args[0], "cd") == 0)
		return (builtin_cd(cmd->args, env));
	if (ft_strcmp(cmd->args[0], "exit") == 0)
		return (builtin_exit(cmd->args));
	if (ft_strcmp(cmd->args[0], "export") == 0)
		return (builtin_export(cmd->args, env));
	if (ft_strcmp(cmd->args[0], "unset") == 0)
		return (builtin_unset(cmd->args, env));
	return (1);
}

static int	exec_builtin(t_command *cmd, char ***env)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	if (ft_strcmp(cmd->args[0], "echo") == 0)
		return (builtin_echo(cmd->args));
	if (ft_strcmp(cmd->args[0], "pwd") == 0)
		return (builtin_pwd());
	if (ft_strcmp(cmd->args[0], "env") == 0)
		return (builtin_env(*env));
	return (1);
}

int	exec_builtin_or_parent(t_command *cmd, char ***env)
{
	int	status;

	status = 0;
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	if (ft_strcmp(cmd->args[0], "cd") == 0
		|| ft_strcmp(cmd->args[0], "exit") == 0
		|| ft_strcmp(cmd->args[0], "export") == 0
		|| ft_strcmp(cmd->args[0], "unset") == 0)
		status = exec_builtin_parent(cmd, env);
	else
		status = exec_builtin(cmd, env);
	g_exit_code(status);
	return (status);
}

int	execute_single_command(t_command *cmd, char ***env)
{
	int	status;
	int	save_in;
	int	save_out;

	status = 0;
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	save_in = dup(STDIN_FILENO);
	save_out = dup(STDOUT_FILENO);
	if (apply_redirs(cmd, env) != 0)
	{
		close(save_in);
		close(save_out);
		return (1);
	}
	if (is_builtin_command(cmd))
		status = exec_builtin_or_parent(cmd, env);
	else
		status = run_external_command(cmd, env);
	dup2(save_in, STDIN_FILENO);
	dup2(save_out, STDOUT_FILENO);
	close(save_in);
	close(save_out);
	g_exit_code(status);
	return (status);
}

static int	count_words(const char *s, char c)
{
	int	count;

	count = 0;
	while (*s)
	{
		while (*s && *s == c)
			s++;
		if (*s)
		{
			count++;
			while (*s && *s != c)
				s++;
		}
	}
	return (count);
}
static void	*free_partial(char **arr, int i)
{
	while (i >= 0)
		free(arr[i--]);
	free(arr);
	return (NULL);
}

static int	fill_word(char ***result, const char **s, char c, int i)
{
	int	len;

	while (**s && **s == c)
		(*s)++;
	len = 0;
	while ((*s)[len] && (*s)[len] != c)
		len++;
	(*result)[i] = ft_substr(*s, 0, len);
	if (!(*result)[i])
		return (0);
	*s += len;
	return (1);
}

static char	**ft_split(const char *s, char c)
{
	char	**result;
	int		words;
	int		i;

	if (!s)
		return (NULL);
	words = count_words(s, c);
	result = malloc(sizeof(char *) * (words + 1));
	if (!result)
		return (NULL);
	i = 0;
	while (i < words)
	{
		if (!fill_word(&result, &s, c, i))
			return (free_partial(result, i - 1));
		i++;
	}
	result[i] = NULL;
	return (result);
}

static void	free_str_array(char **arr)
{
	int	i;

	if (!arr)
		return;
	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}
static char	*build_path(char *dir, char *cmd)
{
	char	*tmp;
	char	*full_path;

	tmp = ft_strjoin(dir, "/");
	if (!tmp)
		return (NULL);
	full_path = ft_strjoin(tmp, cmd);
	free(tmp);
	return (full_path);
}
static char	*check_and_return(char *full_path, char **path)
{
	if (access(full_path, X_OK) == 0)
	{
		free_str_array(path);
		return (full_path);
	}
	free(full_path);
	return (NULL);
}
char	*find_in_path(char *cmd, char **env)
{
	char	**path;
	char	*full_path;
	int		i;

	path = ft_split(get_env_value("PATH", env), ':');
	if (!path)
		return (NULL);
	i = 0;
	while (path[i])
	{
		full_path = build_path(path[i], cmd);
		if (!full_path)
			return (free_str_array(path), NULL);
		full_path = check_and_return(full_path, path);
		if (full_path)
			return (full_path);
		i++;
	}
	free_str_array(path);
	return (NULL);
}

static int	command_not_found(char *cmd)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd, 2);
	ft_putstr_fd(": command not found\n", 2);
	return (127);
}

static void	execve_command(char *path, t_command *cmd, char **env)
{
	execve(path, cmd->args, env);
	perror("execve");
	exit(126);
}

static int	check_command_path(char *path)
{
	struct stat	s;

	if (access(path, F_OK) != 0)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(path, STDERR_FILENO);
		ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
		return (127);
	}
	if (stat(path, &s) == 0 && S_ISDIR(s.st_mode))
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(path, STDERR_FILENO);
		ft_putstr_fd(": Is a directory\n", STDERR_FILENO);
		return (126);
	}
	return (0);
}

static char	*command_path(t_command *cmd, char ***env, int *status)
{
	char	*path;

	if (ft_strchr(cmd->args[0], '/'))
	{
		*status = check_command_path(cmd->args[0]);
		if (*status != 0)
			return (NULL);
		return (ft_strdup(cmd->args[0]));
	}
	path = find_in_path(cmd->args[0], *env);
	if (!path)
	{
		*status = command_not_found(cmd->args[0]);
		return (NULL);
	}
	*status = 0;
	return (path);
}

int	run_external_command(t_command *cmd, char ***env)
{
	pid_t	pid;
	int		status;
	char	*path;

	path = command_path(cmd, env, &status);
	if (!path)
		return (status);
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		free(path);
		return (1);
	}
	if (pid == 0)
		execve_command(path, cmd, *env);
	waitpid(pid, &status, 0);
	free(path);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (1);
}

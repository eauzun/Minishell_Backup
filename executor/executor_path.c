#include "../minishell.h"

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

int	command_not_found(char *cmd)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd, 2);
	ft_putstr_fd(": command not found\n", 2);
	return (127);
}

void	execve_command(char *path, t_command *cmd, char **env)
{
	execve(path, cmd->args, env);
	perror("execve");
	exit(126);
}
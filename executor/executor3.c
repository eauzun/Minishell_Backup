#include "../minishell.h"

int	check_command_path(char *path)
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

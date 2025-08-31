#include "../minishell.h"

static char	*build_path(char *dir, char *cmd)
{
	char	*temp;
	char	*path;

	temp = ft_strjoin(dir, "/");
	if (!temp)
		return (NULL);
	path = ft_strjoin(temp, cmd);
	free(temp);
	return (path);
}

static int	is_executable(char *path)
{
	struct stat	st;

	if (stat(path, &st) == 0 && (st.st_mode & S_IXUSR))
		return (1);
	return (0);
}

char	*find_in_path(char *cmd, char **env)
{
	char	*path_env;
	char	**paths;
	char	*full_path;
	int		i;

	path_env = get_env_value("PATH", env);
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		full_path = build_path(paths[i], cmd);
		if (full_path && is_executable(full_path))
		{
			return (free_str_array(paths),full_path);
		}
		if (full_path)
			free(full_path);
		i++;
	}
	free_str_array(paths);
	return (NULL);
}

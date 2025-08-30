#include "../minishell.h"

int	contains_slash(const char *str)
{
	size_t	i;

	i = 0;
	while (str && str[i])
	{
		if (str[i] == '/')
			return (1);
		i++;
	}
	return (0);
}

char	*path_join(const char *dir, const char *cmd)
{
	char	*new_dir;
	char	*path;

	new_dir = ft_strjoin(dir, "/");
	if (!new_dir)
		return (NULL);
	path = ft_strjoin(new_dir, cmd);
	free(new_dir);
	return (path);
}

char	*check_path_seg(const char *path, size_t start, size_t end,
								const char *cmd)
{
	char	*seg;
	char	*cand;

	seg = ft_substr(path, start, end - start);
	if (!seg)
		return (NULL);
	cand = path_join(seg, cmd);
	free(seg);
	if (cand && access(cand, F_OK) == 0 && access(cand, X_OK) == 0)
		return (cand);
	if (cand)
		free(cand);
	return (NULL);
}

char	*find_in_path(const char *cmd, char **env)
{
	char	*path;
	size_t	i;
	size_t	j;
	char	*result;

	path = get_env_value("PATH", env);
	if (!path)
		return (NULL);
	i = 0;
	while (path[i])
	{
		j = i;
		while (path[j] && path[j] != ':')
			j++;
		result = check_path_seg(path, i, j, cmd);
		if (result)
			return (result);
		if (path[j] == ':')
			i = j + 1;
		else
			i = j;
	}
	return (NULL);
}

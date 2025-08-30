#include "../minishell.h"

int	is_dir(const char *path)
{
	struct stat	st;

	return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

int	is_reg(const char *path)
{
	struct stat	st;

	return (stat(path, &st) == 0 && S_ISREG(st.st_mode));
}

char	**make_sh_argv(t_command *cmd, const char *prog)
{
	int		ac;
	int		i;
	char	**av;

	ac = count_args(cmd->args);
	av = (char **)malloc(sizeof(char *) * (ac + 2));
	if (!av)
		return (NULL);
	av[0] = "/bin/sh";
	av[1] = (char *)prog;
	i = 1;
	while (i + 1 < ac + 1)
	{
		av[i + 1] = cmd->args[i];
		i++;
	}
	av[ac + 1] = NULL;
	return (av);
}

void	print_error(const char *a, const char *b, const char *c)
{
	ft_putstr_fd(a, 2);
	ft_putstr_fd(b, 2);
	ft_putstr_fd(c, 2);
}

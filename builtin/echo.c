#include "../minishell.h"

static int	is_n_flag(char *arg)
{
	int	i;

	if (!arg || arg[0] != '-' || arg[1] != 'n')
		return (0);
	i = 2;
	while (arg[i])
	{
		if (arg[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

static int	check_n_flags(char **args, int *start_index)
{
	int	n_flag;
	int	i;

	n_flag = 0;
	i = 1;
	while (args[i] && is_n_flag(args[i]))
	{
		n_flag = 1;
		i++;
	}
	*start_index = i;
	return (n_flag);
}

int	builtin_echo(char **args)
{
	int	i;
	int	n_flag;

	n_flag = check_n_flags(args, &i);
	while (args[i])
	{
		if (write(1, args[i], ft_strlen(args[i])) == -1)
			return (1);
		if (args[i + 1])
		{
			if (write(1, " ", 1) == -1)
				return (1);
		}
		i++;
	}
	if (!n_flag)
	{
		if (write(1, "\n", 1) == -1)
			return (1);
	}
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hialpagu <hialpagu@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 18:29:04 by hialpagu          #+#    #+#             */
/*   Updated: 2025/09/03 07:23:30 by hialpagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	exit_numeric(char *arg)
{
	long long	n;
	int			code;

	n = ft_atoll(arg);
	code = 256 + (int)((unsigned char)n);
	return (g_exit_code(code));
}

static int	exit_non_numeric(char *arg)
{
	ft_putstr_fd("minishell: exit: ", 2);
	ft_putstr_fd(arg, 2);
	ft_putstr_fd(": numeric argument required\n", 2);
	return (g_exit_code(258));
}

static int	exit_too_many(void)
{
	ft_putstr_fd("minishell: exit: too many arguments\n", 2);
	return (g_exit_code(257));
}

static int	exit_default(void)
{
	int	code;

	code = g_exit_code(-1);
	if (code == 0)
		code = 256;
	else
		code = 256 + code;
	return (g_exit_code(code));
}

int	builtin_exit(char **args)
{
	int	argc;

	argc = 0;
	while (args && args[argc])
		argc++;
	if (argc == 1)
	{
		ft_putstr_fd("exit\n", 1);
		return (exit_default());
	}
	if (!is_numeric(args[1]))
		return (exit_non_numeric(args[1]));
	if (argc > 2)
		return (exit_too_many());
	ft_putstr_fd("exit\n", 1);
	return (exit_numeric(args[1]));
}

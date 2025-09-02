/* Güncellenmiş builtin/exit.c */
#include "../minishell.h"

static int is_numeric(char *str)
{
    int i;

    i = 0;
    if (str[i] == '-' || str[i] == '+')
        i++;
    if (!str[i])
        return (0);
    while (str[i])
    {
        if (str[i] < '0' || str[i] > '9')
            return (0);
        i++;
    }
    return (1);
}

static long long ft_atoll(char *str)
{
    long long result;
    int sign;
    int i;

    result = 0;
    sign = 1;
    i = 0;
    if (str[i] == '-' || str[i] == '+')
    {
        if (str[i] == '-')
            sign = -1;
        i++;
    }
    while (str[i] >= '0' && str[i] <= '9')
    {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    return (result * sign);
}

static int handle_exit_cases(char **args, int argc)
{
    long long n;
    int exit_code;

    if (argc == 1)
    {
        exit_code = g_exit_code(-1);
        if (exit_code == 0)
            exit_code = 256; /* Special marker: normal exit */
        else
            exit_code = 256 + exit_code; /* Keep original exit code */
        return (g_exit_code(exit_code));
    }
    if (!is_numeric(args[1]))
    {
        ft_putstr_fd("minishell: exit: ", 2);
        ft_putstr_fd(args[1], 2);
        ft_putstr_fd(": numeric argument required\n", 2);
        return (g_exit_code(258)); /* 256 + 2 */
    }
    if (argc > 2)
    {
        ft_putstr_fd("minishell: exit: too many arguments\n", 2);
        return (g_exit_code(257)); /* 256 + 1, but don't exit */
    }
    n = ft_atoll(args[1]);
    exit_code = 256 + (int)((unsigned char)n);
    return (g_exit_code(exit_code));
}

int builtin_exit(char **args)
{
    int argc;

    ft_putstr_fd("exit\n", 1);
    argc = 0;
    while (args && args[argc])
        argc++;
    return (handle_exit_cases(args, argc));
}
#include "../minishell.h"

int	g_exit_code(int code)
{
	static int	temp;

	if (code != -1)
		temp = code;
	return (temp);
}

void	signal_handler(int signal)
{
	(void)signal;
	write(1, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
	g_exit_code(130);
}

void	set_signals(void)
{
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, SIG_IGN);
}

void	set_child_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
int g_should_exit(int set)
{
    static int flag = 0;    // 0: devam, 1: çık

    if (set >= 0)
        flag = set;
    return (flag);
}

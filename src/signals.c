#include "../minishell.h"


int g_exit_code(int code)
{
    static int temp = 0; 
    
    if (code != -1)
        temp = code;
    return (temp);
}


int should_exit_program(void)
{
    return (g_exit_code(-1) > 0);
}

void signal_handler(int signal)
{
    (void)signal;
    write(1, "\n", 1);
    rl_replace_line("", 0);
    rl_on_new_line();
    rl_redisplay();
    g_exit_code(130);
}

void set_signals(void)
{
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, SIG_IGN);
}

void set_child_signals(void)
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
}

void heredoc_signal_handler(int sig)
{
    (void)sig;
    write(1, "\n", 1);
    g_exit_code(130);
    rl_replace_line("", 0);
    rl_done = 1;
}
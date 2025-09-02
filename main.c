/* Güncellenmiş main.c */
#include "minishell.h"

/* Yardımcı fonksiyon: exit gerekip gerekmediğini kontrol eder */
static int check_should_exit(void)
{
    int code = g_exit_code(-1);
    
    /* 256 ve üzeri: exit komutu çalıştırıldı */
    return (code >= 256);
}

/* Gerçek exit code'u döndürür (256'dan fazlasını düzelt) */
static int get_real_exit_code(void)
{
    int code = g_exit_code(-1);
    
    if (code >= 256)
        return (code - 256);
    return (code);
}

int run_command_line(t_command *cmds, char ***env, t_token *tokens)
{
    if (!cmds)
    {
        g_exit_code(0);
        return (0);
    }
    if (cmds->next)
        return (run_pipeline(cmds, env, tokens));
    return (execute_single_command(cmds, env));
}

void execute_line(const char *line, char ***env, int *exit_status)
{
    t_token *tokens;
    t_command *commands;
    int status;

    status = 0;
    tokens = NULL;
    commands = NULL;
    tokens = lexer_init(line);
    if (!tokens)
        return ;
    tokens = expand_tokens(tokens, *env);
    if (tokens)
        tokens = remove_empty_unquoted(tokens);
    if (tokens)
        commands = parse_tokens(tokens);
    if (commands)
    {
        status = run_command_line(commands, env, tokens);
        *exit_status = status;
        g_exit_code(status);
    }
    if (commands)
        free_commands(commands);
    if (tokens)
        free_token(tokens);
}

int minishell(char ***env)
{
    char *line;
    int exit_status;

    exit_status = g_exit_code(-1);
    while (1)
    {
        line = readline("minishell$ ");
        if (!line)
        {
            write(1, "exit\n", 5);
            return (get_real_exit_code());
        }
        if (line[0] && !is_only_space(line))
        {
            add_history(line);
            execute_line(line, env, &exit_status);
            
            /* Tek global değişken kontrolü */
            if (check_should_exit())
            {
                free(line);
                return (get_real_exit_code());
            }
        }
        free(line);
    }
}

int main(int ac, char **av, char **env)
{
    char **env_cpy;
    int status;

    if (ac != 1 || av[1] != NULL)
    {
        write(2, "minishell: no arguments allowed!\n", 33);
        return (1);
    }
    env_cpy = cpy_env_data(env);
    if (!env_cpy)
    {
        write(2, "minishell: environment copy failed!\n", 36);
        return (1);
    }
    set_signals();
    status = minishell(&env_cpy);
    rl_clear_history();
    free_env_data(env_cpy);
    return (status);
}
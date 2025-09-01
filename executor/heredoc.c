#include "../minishell.h"

static int    read_heredoc_line(char *delimiter, int expand, char **env
    int write_fd)
{
    char    *line;
    char    *expanded;
    int initial_exit_code;

    initial_exit_code = g_exit_code(-1);
    while (1)
    {
        line = readline("> ");
        if (!line) // EOF
            {
                write(2, "minishell: warning: here-document delimited by end-of-file\n", 55);
                write(2, delimiter, ft_strlen(delimiter));
                write(2, "')\n", 3);
                return(0);
            }
            if(g_exit_code(-1) == 130) // SIGINT
            {
                free(line);
                return (1);
            }
        if (ft_strcmp(line, delimiter) == 0)
        {
            free(line);
            g_exit_code(initial_exit_code);
            return (0);
        }
        if (expand)
        {
            expanded = expand_string(line, env);
            free(line);
            if (!expanded)
                return (1);
            line = expanded;
        }
        write(write_fd, line, ft_strlen(line));
        write(write_fd, "\n", 1);
        free(line);
    }
}

static int setup_heredoc_pipe(t_heredoc *heredoc)
{
    if(pipe(heredoc->pipe_fd) == -1)
    {
        perror("pipe");
        return (1);
    }
    return (0);
}

int process_single_heredoc(t_heredoc *heredoc, char **env)
{
    void    (*old_handler)(int);
    int     status;
    
    if (setup_heredoc_pipe(heredoc))
        return (1);
    old_handler = signal(SIGINT, heredoc_signal_handler);
    status = read_heredoc_line(heredoc->limiter, heredoc->expand, env, heredoc->pipe_fd[1]);
    close(heredoc->pipe_fd[1]); // close write end after writing
    signal(SIGINT, old_handler);
    if(status == 1) // interrupted by SIGINT
    {
        close(heredoc->pipe_fd[0]);
        return (status);
    }
    return (0);
}

int process_heredocs(t_command *cmd, char **env)
{
    t_heredoc *cur;
    t_heredoc *last_valid;

    if(!cmd || !cmd->heredocs)
        return (0);
    cur = cmd->heredocs;
    last_valid = NULL;
    while(cur)
    {
        if(process_single_heredoc(cur, env) == 1) // interrupted by SIGINT
        {
            if(g_exit_code(-1) == 130)
                return(130);
            return (1);
        }
        if(last_valid && last_valid->pipe_fd[0] != -1)
            close(last_valid->pipe_fd[0]);
        last_valid = cur;
        cur = cur->next;
    }
    if(last_valid)
        dup2(last_valid->pipe_fd[0], STDIN_FILENO);
    return (0);    
    }

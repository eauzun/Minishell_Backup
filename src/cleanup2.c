#include "../minishell.h"

void cleanup_on_parse_error(t_token *tokens, t_command *partial_cmd)
{
    if (tokens)
        free_token(tokens);
    if (partial_cmd)
        free_commands(partial_cmd);
}

void cleanup_on_expand_error(t_token *tokens)
{
    if (tokens)
        free_token(tokens);
}

static void cleanup_and_return(t_token *tokens, t_command *commands)
{
    if (commands)
        free_commands(commands);
    if (tokens)
        free_token(tokens);
    return;        
}
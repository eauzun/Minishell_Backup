#include "../minishell.h"

void	cleanup_tokens_and_commands(t_token *tokens, t_command *commands)
{
	if (tokens)
		free_token(tokens);
	if (commands)
		free_commands(commands);
}

t_token	*process_lexer(const char *line)
{
	t_token	*tokens;

	tokens = lexer_init(line);
	if (!tokens)
		return (NULL);
	return (tokens);
}

t_token	*process_expander(t_token *tokens, char **env)
{
	t_token	*expanded_tokens;

	if (!tokens)
		return (NULL);
	expanded_tokens = expand_tokens(tokens, env);
	if (!expanded_tokens)
	{
		free_token(tokens);
		return (NULL);
	}
	return (expanded_tokens);
}

t_token	*process_quote_removal(t_token *tokens)
{
	t_token	*processed_tokens;

	if (!tokens)
		return (NULL);
	processed_tokens = remove_empty_unquoted(tokens);
	if (!processed_tokens)
	{
		free_token(tokens);
		return (NULL);
	}
	return (processed_tokens);
}

t_command	*process_parser(t_token *tokens)
{
	t_command	*commands;

	if (!tokens)
		return (NULL);
	commands = parse_tokens(tokens);
	return (commands);
}
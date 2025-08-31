#include "../minishell.h"

char	*join_and_free(char *s1, char *s2)
{
	char	*result;

	result = ft_strjoin(s1, s2);
	free(s1);
	free(s2);
	return (result);
}

char	*get_expanded_str(t_token *token, char **env)
{
	if (token->type == T_WORD_SINGLE)
		return (expand_single_quote(token->str));
	else if (token->type == T_WORD_DOUBLE)
		return (expand_double_quote(token->str, env));
	else if (token->type == T_WORD)
		return (expand_regular_word(token->str, env));
	return (NULL);
}

t_token	*expand_tokens(t_token *tokens, char **env)
{
	t_token	*cur;
	char	*new_str;

	cur = tokens;
	while (cur)
	{
		new_str = get_expanded_str(cur, env);
		if (new_str)
		{
			free(cur->str);
			cur->str = new_str;
		}
		else if (cur->type <= T_WORD_DOUBLE)
			return (NULL);
		cur = cur->next;
	}
	return (tokens);
}

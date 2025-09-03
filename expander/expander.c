/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hialpagu <hialpagu@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 16:03:47 by emuzun            #+#    #+#             */
/*   Updated: 2025/09/03 05:50:37 by hialpagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*get_expanded_str(t_token *token, char **env)
{
	if (token->type == T_WORD_SINGLE)
		return (ft_strdup(token->str));
	else if (token->type == T_WORD_DOUBLE)
		return (expand_string(token->str, env));
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

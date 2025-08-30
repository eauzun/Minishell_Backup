#include "../minishell.h"

void	drop_node(t_token **prev, t_token **cur, t_token **new_head)
{
	t_token	*next;

	next = (*cur)->next;
	if (*prev)
		(*prev)->next = next;
	else
		*new_head = next;
	free((*cur)->str);
	free(*cur);
	*cur = next;
}

t_token	*remove_empty_unquoted(t_token *head)
{
	t_token	*prev;
	t_token	*cur;
	t_token	*new_head;

	prev = NULL;
	cur = head;
	new_head = head;
	while (cur)
	{
		if (cur->type == T_WORD && (!cur->str || !cur->str[0]))
		{
			drop_node(&prev, &cur, &new_head);
			continue ;
		}
		prev = cur;
		cur = cur->next;
	}
	return (new_head);
}

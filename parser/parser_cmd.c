#include "../minishell.h"

t_command	*create_command(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->args = malloc(sizeof(char *) * 1);
	if (!cmd->args)
	{
		free(cmd);
		return (NULL);
	}
	cmd->args[0] = NULL;
	cmd->infile = NULL;
	cmd->outfile = NULL;
	cmd->append = 0;
	cmd->heredoc = 0;
	cmd->heredoc_expand = 0;
	cmd->heredoc_prev = NULL;
	cmd->redir_error = 0;
	cmd->redirs = NULL;
	cmd->heredocs = NULL; // YENİ
	cmd->next = NULL;
	return (cmd);
}

void free_redirs(t_redir *redir)
{
    t_redir *next;

    while (redir)
    {
        next = redir->next;
        if (redir->file)
            free(redir->file);
        free(redir);
        redir = next;
    }
}


void	free_commands(t_command *cmd)
{
	t_command	*tmp;
	int			i;

	while (cmd)
	{
		tmp = cmd->next;
		if (cmd->redirs)
		{
			free_redirs(cmd->redirs);
			cmd->redirs = NULL;
		}
		if (cmd->heredocs)                    // YENİ
		{
			free_heredocs(cmd->heredocs);     // YENİ
			cmd->heredocs = NULL;             // YENİ
		}
		if (cmd->args)
		{
			i = 0;
			while (cmd->args[i])
				free(cmd->args[i++]);
			free(cmd->args);
			cmd->args = NULL;
		}
		if (cmd->infile)
			free(cmd->infile);
		if (cmd->outfile)
			free(cmd->outfile);
		if (cmd->heredoc_prev)
			free(cmd->heredoc_prev);
		free(cmd);
		cmd = tmp;
	}
}


void	link_commands(t_command **head, t_command *new_cmd)
{
	t_command	*cur;

	if (!*head)
		*head = new_cmd;
	else
	{
		cur = *head;
		while (cur->next)
			cur = cur->next;
		cur->next = new_cmd;
	}
}

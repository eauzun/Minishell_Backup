#include "../minishell.h"

static void	free_args(char **args)
{
	int	i;

	if (!args)
		return ;
	i = 0;
	while (args[i])
		free(args[i++]);
	free(args);
}

static void	free_cmd_fields(t_command *cmd)
{
	if (cmd->redirs)
	{
		free_redirs(cmd->redirs);
		cmd->redirs = NULL;
	}
	if (cmd->heredocs)
	{
		free_heredocs(cmd->heredocs);
		cmd->heredocs = NULL;
	}
	if (cmd->args)
	{
		free_args(cmd->args);
		cmd->args = NULL;
	}
	if (cmd->infile)
		free(cmd->infile);
	if (cmd->outfile)
		free(cmd->outfile);
}

void	free_commands(t_command *cmd)
{
	t_command	*tmp;

	while (cmd)
	{
		tmp = cmd->next;
		free_cmd_fields(cmd);
		free(cmd);
		cmd = tmp;
	}
}

void	free_redirs(t_redir *redir)
{
	t_redir	*next;

	while (redir)
	{
		next = redir->next;
		if (redir->file)
			free(redir->file);
		free(redir);
		redir = next;
	}
}

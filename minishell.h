#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <string.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <signal.h>
# include <stddef.h>
# include <limits.h>
# include <stdbool.h>
# include <sys/stat.h>
# include <limits.h>

typedef enum e_token_type
{
	T_WORD,
	T_WORD_SINGLE,
	T_WORD_DOUBLE,
	T_PIPE,
	T_REDIR_IN,
	T_REDIR_OUT,
	T_APPEND,
	T_HEREDOC,
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*str;
	int				sep_ws;
	struct s_token	*next;
}	t_token;

typedef enum e_redir_type
{
	R_IN,
	R_OUT,
	R_APPEND,
}	t_redir_type;

typedef struct s_redir
{
	t_redir_type	type;
	char			*file;
	struct s_redir	*next;
}	t_redir;

typedef struct s_heredoc
{
	char			*delimiter;
	int				expand_vars;
	int				pipe_fd[2];
	struct s_heredoc	*next;
}	t_heredoc;

typedef struct s_command
{
	int					redir_error;
	int					append;
	int					heredoc;
	char				**args;
	char				*infile;
	char				*outfile;
	t_redir				*redirs;
	t_heredoc			*heredocs;
	struct s_command	*next;
}	t_command;

typedef struct s_pipe_info
{
	int		n;
	int		*pipes;
	pid_t	*pids;
	char	***env;
}	t_pipe_info;

// MAIN.C
int			run_command_line(t_command *cmds, char ***env, t_token *tokens);
void		execute_line(const char *line, char ***env, int *exit_status);
int			minishell(char ***env);
//  SRC/ENV_DATA.C
char		**cpy_env_data(char **env);
void		free_env_data(char **env);
//  SRC/SIGNALS.C
int			g_exit_code(int code);
void		signal_handler(int signal);
void		set_signals(void);
void		set_child_signals(void);
int			g_should_exit(int set);

//  UTILS/UTILS.C
size_t		ft_strlen(const char *s);
char		*ft_strdup(const char *s);
bool		is_only_space(const char *s);
char		*collect_filename_words(t_token **cur_t);
size_t		ft_strcpy(char *dst, const char *src);
//  UTILS/UTILS2.C
char		*ft_strjoin(char const *s1, char const *s2);
char		*ft_substr(char const *s, unsigned int start, size_t len);
int			ft_strcmp(const char *s1, const char *s2);
int			ft_strncmp(const char *s1, const char *s2, size_t n);
size_t		ft_strncpy(char *dst, const char *src, size_t n);
//  UTILS/UTILS3.C
char		*ft_itoa(int n);
int			ft_isalnum(int c);
char		*ft_strchr(const char *s, int c);
char		*ft_strrchr(const char *str, int c);

//  LEXER/LEXER.C
t_token		*lexer_init(const char *line);
//  LEXER/LEXER_UTILS.C
t_token		*new_token(t_token_type type, const char *start, size_t len);
void		add_token(t_token **list, t_token *token);
void		free_token(t_token *list);
void		skip_whitespace(const char *line, size_t *i);
//  LEXER/LEXER_SPECIAL.C
t_token		*single_quote(const char *line, size_t *i);
t_token		*double_quote(const char *line, size_t *i);
t_token		*redirections(const char *line, size_t *i);
t_token		*is_pipe(const char *line, size_t *i);

//  EXPAND/EXPANDER.C
char		*join_and_free(char *s1, char *s2);
t_token		*expand_tokens(t_token *tokens, char **env);
//  EXPANDER/EXPANDER_VARIABLES.C
char		*get_env_value(char *name, char **env);
char		*handle_dollar(char *str, size_t *i, char **env);
//  EXPANDER/EXPANDER_UTILS.C
char		*quoted_next_segment(const char *str, size_t *i, char **env,
				char quote);
char		*expand_in_quotes(const char *str, size_t *i, char **env,
				char quote);
char		*expand_next_segment(const char *str, size_t *i, char **env);
char		*expand_string(const char *str, char **env);
//  EXPANDER/EXPANDER_TYPE.C
char		*expand_double_quote(const char *str, char **env);
char		*expand_regular_word(char *str, char **env);
char		*expand_single_quote(char *str);
//  EXPANDER/EXPANDER_QUOTE.C
t_token		*remove_empty_unquoted(t_token *head);

//  PARSER/PARSER.C
t_command	*parse_tokens(t_token *tokens);
//  PARSER/PARSER_VALID.C
int			is_word(t_token *token);
int			validate_syntax(t_token *tokens);
//  PARSER/PARSER_REDIR.C
void		parse_redirections(t_token **cur, t_command *cmd);
//  PARSER/PARSER_UTILS.C
void		syntax_error(const char *msg);
void		add_argument(t_command *cmd, char *arg);
void		add_redir(t_command *cmd, t_redir_type type, char *file);
//	PARSER/PARSER_FREE_UTILS.C
void		free_commands(t_command *cmd);
void		free_redirs(t_redir *redir);
//  PARSER/PARSER_CMD.C
t_command	*create_command(void);
void		link_commands(t_command **head, t_command *new_cmd);

//  BUILTIN/CD.C
int			builtin_cd(char **args, char ***env);
void		ft_putstr_fd(char *s, int fd);

//  BUILTIN/CD_UTILS.C
char		*get_home_path(char **env);
char		*get_oldpwd_path(char **env);
char		*get_target_path(char **args, char **env);
int			update_pwd_vars(char ***env, char *old_pwd, char *new_pwd);
//  BUILTIN/ECHO.C
int			builtin_echo(char **args);
//  BUILTIN/ENV.C
int			builtin_env(char **env);
//  BUILTIN/EXIT.C
int			builtin_exit(char **args);
//  BUILTIN/EXPORT.C
int			builtin_export(char **args, char ***env);
//  BUILTIN/EXPORT_UTILS.C
int			is_valid_identifier(char *str);
int			find_equals_position(char *str);
void		parse_assignment(char *arg, char **name, char **value);
int			set_env_var(char ***env, char *name, char *value);
//  BUILTIN/EXPORT_UTILS1.C
int			find_env_var(char **env, char *name);
int			update_env_var(char ***env, int pos, char *name, char *value);
char		*create_env_string(char *name, char *value);
int			get_env_size(char **env);
//  BUILTIN/EXPORT_UTILS2.C
void		free_env_array(char **env);
char		**copy_env_array(char **env);
char		**sort_env_for_export(char **env);
//  BUILTIN/EXPORT_UTILS3.C
int			print_export_line(char *line);
char		**add_env_var(char **env, char *name, char *value);
//  BUILTIN/PWD.C
int			builtin_pwd(void);
//  BUILTIN/UNSET.C
int			builtin_unset(char **args, char ***env);



int			execute_single_command(t_command *cmd, char ***env);
int			run_external_command(t_command *cmd, char ***env);
int			exec_builtin_or_parent(t_command *cmd, char ***env);
int			apply_redirs(t_command *cmd, char ***env);
char    	*find_in_path(char *cmd, char **env);
int     	is_builtin_command(t_command *cmd);
int			run_pipeline(t_command *cmds, char ***env, t_token *tokens);
int			check_command_path(char *path);
char 		*find_in_path(char *cmd, char **env);
int			setup_command_fds(t_command *cmd, char ***env, int *save_fds);
void		close_all_pipes(int *pipes, int count);
char    	**ft_split(char const *s, char c);


int	read_heredoc_line(char *delimiter, int expand, char **env, int write_fd);
int process_heredocs(t_command *cmd, char **env);
int		process_single_heredoc(t_heredoc *heredoc, char **env);
t_heredoc	*create_heredoc(char *delimiter, int expand_vars);
void	free_heredocs(t_heredoc *heredocs);
void	add_heredoc(t_command *cmd, char *delimiter, int expand_vars);
void	cleanup_heredoc_interrupt(t_command *cmd);
void	heredoc_signal_handler(int sig);
void	cleanup_child(t_command *cmd, t_token *tokens, t_pipe_info *info, int exit_code);
#endif
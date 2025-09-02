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
# include <sys/stat.h>
# include <signal.h>
# include <stddef.h>
# include <limits.h>
# include <stdbool.h>

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

typedef enum e_redir_type
{
	R_IN,
	R_OUT,
	R_APPEND,
}	t_redir_type;

typedef struct s_token
{
	t_token_type	type;
	char			*str;
	int				sep_ws;
	struct s_token	*next;
}	t_token;

typedef struct s_redir
{
	t_redir_type	type;
	char			*file;
	struct s_redir	*next;
}	t_redir;

typedef struct s_heredoc
{
	char				*delimiter;
	int					expand_vars;
	int					pipe_fd[2];
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

int			main(int ac, char **av, char **env);
int			minishell(char ***env);
void		execute_line(const char *line, char ***env, int *exit_status);
int			run_command_line(t_command *cmds, char ***env, t_token *tokens);
char		**cpy_env_data(char **env);
void		free_env_data(char **env);
int			g_exit_code(int code);
int			g_should_exit(int set);
void		signal_handler(int signal);
void		set_signals(void);
void		set_child_signals(void);
t_token		*lexer_init(const char *line);
t_token		*single_quote(const char *line, size_t *i);
t_token		*double_quote(const char *line, size_t *i);
t_token		*redirections(const char *line, size_t *i);
t_token		*is_pipe(const char *line, size_t *i);
t_token		*new_token(t_token_type type, const char *start, size_t len);
void		add_token(t_token **list, t_token *token);
void		free_token(t_token *list);
void		skip_whitespace(const char *line, size_t *i);
char		*join_and_free(char *s1, char *s2);
t_token		*expand_tokens(t_token *tokens, char **env);
t_token		*remove_empty_unquoted(t_token *head);
char		*expand_single_quote(char *str);
char		*expand_double_quote(const char *str, char **env);
char		*expand_regular_word(char *str, char **env);
char		*quoted_next_segment(const char *str, size_t *i,
				char **env, char quote);
char		*expand_in_quotes(const char *str, size_t *i,
				char **env, char quote);
char		*expand_next_segment(const char *str, size_t *i, char **env);
char		*expand_string(const char *str, char **env);
char		*get_env_value(char *name, char **env);
char		*handle_dollar(char *str, size_t *i, char **env);
t_command	*parse_tokens(t_token *tokens);
t_command	*create_command(void);
void		link_commands(t_command **head, t_command *new_cmd);
void		free_commands(t_command *cmd);
void		free_redirs(t_redir *redir);
void		parse_redirections(t_token **cur, t_command *cmd);
void		syntax_error(const char *msg);
void		add_argument(t_command *cmd, char *arg);
void		add_redir(t_command *cmd, t_redir_type type, char *file);
int			is_word(t_token *token);
int			validate_syntax(t_token *tokens);
int			builtin_cd(char **args, char ***env);
char		*get_home_path(char **env);
char		*get_oldpwd_path(char **env);
char		*get_target_path(char **args, char **env);
int			update_pwd_vars(char ***env, char *old_pwd, char *new_pwd);
void		ft_putstr_fd(char *s, int fd);
int			builtin_echo(char **args);
int			builtin_env(char **env);
int			builtin_exit(char **args);
int			builtin_export(char **args, char ***env);
int			is_valid_identifier(char *str);
int			find_equals_position(char *str);
void		parse_assignment(char *arg, char **name, char **value);
int			set_env_var(char ***env, char *name, char *value);
int			find_env_var(char **env, char *name);
int			update_env_var(char ***env, int pos, char *name, char *value);
char		*create_env_string(char *name, char *value);
int			get_env_size(char **env);
void		free_env_array(char **env);
char		**copy_env_array(char **env);
char		**sort_env_for_export(char **env);
int			print_export_line(char *line);
char		**add_env_var(char **env, char *name, char *value);
int			builtin_pwd(void);
int			builtin_unset(char **args, char ***env);
int			apply_redirs(t_command *cmd, char ***env);
int			execute_single_command(t_command *cmd, char ***env);
int			setup_command_fds(t_command *cmd, char ***env, int *save_fds);
int			is_builtin_command(t_command *cmd);
int			exec_builtin_or_parent(t_command *cmd, char ***env);
int			check_command_path(char *path);
char		*find_in_path(char *cmd, char **env);
int			run_external_command(t_command *cmd, char ***env);
void		heredoc_signal_handler(int sig);
int			process_single_heredoc(t_heredoc *heredoc, char **env);
int			process_heredocs(t_command *cmd, char **env);
t_heredoc	*create_heredoc(char *delimiter, int expand_vars);
void		free_heredocs(t_heredoc *heredocs);
void		add_heredoc(t_command *cmd, char *delimiter, int expand_vars);
void		cleanup_heredoc_interrupt(t_command *cmd);
int			read_heredoc_line(char *delimiter, int expand,
				char **env, int write_fd);
int			run_pipeline(t_command *cmds, char ***env, t_token *tokens);
void		cleanup_child(t_command *cmd, t_token *tokens,
				t_pipe_info *info, int exit_code);
void		close_all_pipes(int *pipes, int count);
void		cleanup_pipe_info(t_pipe_info *info);
void		child_run_builtin(t_command *cmd, t_pipe_info *info,
				t_token *tokens);
void		child_run_external(t_command *cmd, t_pipe_info *info,
				t_token *tokens);
void		child_run(t_command *cmd, t_pipe_info *info, t_token *tokens);
void		child_process(t_command *cmd, int index,
				t_pipe_info *info, t_token *tokens);
int			*setup_pipes(int count);
int			preprocess_pipeline_heredocs(t_command *cmds, char **env);
char		**ft_split(char const *s, char c);
size_t		ft_strlen(const char *s);
char		*ft_strdup(const char *s);
bool		is_only_space(const char *s);
char		*collect_filename_words(t_token **cur_t);
size_t		ft_strcpy(char *dst, const char *src);
char		*ft_strjoin(char const *s1, char const *s2);
char		*ft_substr(char const *s, unsigned int start, size_t len);
int			ft_strcmp(const char *s1, const char *s2);
int			ft_strncmp(const char *s1, const char *s2, size_t n);
size_t		ft_strncpy(char *dst, const char *src, size_t n);
char		*ft_itoa(int n);
int			ft_isalnum(int c);
char		*ft_strchr(const char *s, int c);
char		*ft_strrchr(const char *str, int c);

#endif
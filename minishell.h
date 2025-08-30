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

typedef struct s_command
{
	int					redir_error;
	int					heredoc_expand;
	int					append;
	int					heredoc;
	char				**args;
	char				*infile;
	char				*outfile;
	char				*heredoc_prev;
	t_redir				*redirs;
	struct s_command	*next;
}	t_command;

typedef struct s_pipe_info
{
	int		index;
	int		total;
	int		(*pipes)[2];
}	t_pipe_info;

// MAIN.C
int			run_command_line(t_command *cmds, char ***env);
void		execute_line(const char *line, char ***env, int *exit_status);
int			minishell(char ***env);
//  SRC/ENV_DATA.C
char		**cpy_env_data(char **env);
void		free_env_data(char **env);
//  UTILS/UTILS.C
size_t		ft_strlen(const char *s);
char		*ft_strdup(const char *s);
bool		is_only_space(const char *s);
char		*collect_filename_words(t_token **cur_t);
//  UTILS/UTILS1.C
char		*ft_strjoin(char const *s1, char const *s2);
char		*ft_substr(char const *s, unsigned int start, size_t len);
int			ft_strcmp(const char *s1, const char *s2);
int			ft_strncmp(const char *s1, const char *s2, size_t n);
//  UTILS/UTILS2.C
char		*ft_itoa(int n);
int			ft_isalnum(int c);
char		*ft_strchr(const char *s, int c);
//  SRC/SIGNALS.C
int			g_exit_code(int code);
void		signal_handler(int signal);
void		set_signals(void);
void		set_child_signals(void);
int			g_should_exit(int set);
//  LEXER/LEXER.C
t_token		*lexer_init(const char *line);
t_token		*token_word(const char *line, size_t *i);
t_token		*handle_special(const char *line, size_t *i);
//  LEXER/LEXER_SPECIAL.C
t_token		*single_quote(const char *line, size_t *i);
t_token		*double_quote(const char *line, size_t *i);
t_token		*redirections(const char *line, size_t *i);
t_token		*is_pipe(const char *line, size_t *i);
//  LEXER/LEXER_UTILS.C
void		skip_whitespace(const char *line, size_t *i);
void		free_token(t_token *list);
void		add_token(t_token **list, t_token *token);
t_token		*new_token(t_token_type type, const char *start, size_t len);
//  EXPAND/EXPANDER.C
char		*join_and_free(char *s1, char *s2);
char		*get_expanded_str(t_token *token, char **env);
t_token		*expand_tokens(t_token *tokens, char **env);
//  EXPANDER/EXPANDER_TYPE.C
char		*expand_double_quote(const char *str, char **env);
char		*expand_regular_word(char *str, char **env);
char		*expand_single_quote(char *str);
//  EXPANDER/EXPANDER_VARIABLES.C
char		*get_var_name(char *str, size_t *i);
char		*get_env_value(char *name, char **env);
char		*get_var_value(char *var_name, char **env);
char		*handle_dollar(char *str, size_t *i, char **env);
//  EXPANDER/EXPANDER_UTILS.C
char		*quoted_next_segment(const char *str, size_t *i, char **env,
				char quote);
char		*expand_in_quotes(const char *str, size_t *i, char **env,
				char quote);
char		*expand_next_segment(const char *str, size_t *i, char **env);
char		*expand_string(const char *str, char **env);
//  EXPANDER/EXPANDER_QUOTE.C
void		drop_node(t_token **prev, t_token **cur, t_token **new_head);
t_token		*remove_empty_unquoted(t_token *head);
//  PARSER/PARSER.C
int			is_word(t_token *token);
char		*gather_word(t_token **cur);
t_command	*parse_single_command(t_token **tokens);
t_command	*parse_tokens(t_token *tokens);
//  PARSER/PARSER_CMD.C
t_command	*create_command(void);
void		free_redirs(t_redir *redir);
void		free_commands(t_command *cmd);
void		link_commands(t_command **head, t_command *new_cmd);
//  PARSER/PARSER_VALID.C
int			check_pipe_syntax(t_token *cur);
int			check_redir_syntax(t_token *cur);
int			check_first_token(t_token *tokens);
int			validate_syntax(t_token *tokens);
//  PARSER/PARSER_REDIR.C
char		*join_filename_tokens(t_token **cur, int *was_quoted);
int			get_redir_target(t_token **cur, char **out_name, int *was_quoted);
void		apply_heredoc(t_command *cmd, char *name, int quoted_first_part);
void		parse_redirections(t_token **cur, t_command *cmd);
//  PARSER/PARSER_UTILS.C
void		syntax_error(const char *msg);
int			count_args(char **args);
void		add_argument(t_command *cmd, char *arg);
void		add_redir(t_command *cmd, t_redir_type type, char *file);
//  EXECUTOR/EXECUTOR.C
int			set_wait_status(int status);
int			run_external_command(t_command *cmd, char ***env);
int			exec_builtin_or_parent(t_command *cmd, char ***env);
int			execute_single_command(t_command *cmd, char ***env);
//  EXECUTOR/EXECUTOR2.C
void		restore_stdio(int in_fd, int out_fd);
int			run_child(t_command *cmd, char ***env);
int			execute_builtin_parent(t_command *cmd, char ***env);
void		ft_putstr_fd(const char *s, int fd);
//  EXECUTOR/EXECUTOR_REDIR.C
int			redirect_in(const char *path);
int			redirect_out(const char *path, int append);
int			apply_redirs(t_command *cmd, char ***env);
//  EXECUTOR/EXECUTOR_PIPE.C
void		setup_pipe_ends(int i, int n, int (*pipes)[2], t_command *c);
void		execute_command(t_command *c, char ***env);
void		child_process(t_command *c, t_pipe_info *info, char ***env);
int			wait_all_children(pid_t *pids, int n);
//  EXECUTOR/EXECUTOR_PIPE2.C
int			create_pipes(int (*pipes)[2], int total);
void		spawn_all_children(t_command *cmds, pid_t *pids, t_pipe_info *info,
				char ***env);
int			run_pipeline(t_command *cmds, char ***env);
//  EXECUTOR/EXECUTOR_HEREDOC.C
int			handle_quit(char *line, int *p);
char		*process_line(char *line, char **env, int flag);
int			read_lines(t_command *cur, char **env, int *p);
int			exec_heredoc(t_command *cur, char **env);
//  EXECUTOR/EXECUTOR_UTILS.C
int			contains_slash(const char *str);
char		*path_join(const char *dir, const char *cmd);
char		*check_path_seg(const char *path, size_t start, size_t end,
				const char *cmd);
char		*find_in_path(const char *cmd, char **env);
//  EXECUTOR/EXECUTOR_UTILS2.C
int			is_dir(const char *path);
int			is_reg(const char *path);
char		**make_sh_argv(t_command *cmd, const char *prog);
void		print_error(const char *a, const char *b, const char *c);
//  EXECUTOR/EXECUTOR_UTILS3.C
void		handle_exec_errors(char *prog, t_command *cmd);
void		exec_or_die(char *prog, t_command *cmd, char **env);
int			cmd_count(t_command *cmd);
void		close_all(int (*pipes)[2], int n);
//  EXECUTOR/BUILTIN.C
int			is_builtin(char *cmd);
int			execute_builtin(t_command *cmd, char ***env);
//  BUILTIN/CD.C
int			builtin_cd(char **args, char ***env);
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


#endif
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 16:36:40 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/21 09:51:40 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

/* ========================================================================== */
/*                               INCLUDES                                     */
/* ========================================================================== */

# include <stdio.h>
# include <sys/types.h>
# include <dirent.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <stdlib.h>
# include <string.h>
# include <signal.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <errno.h>
# include "../libft/libft.h"
# include "../libft/get_next_line.h"

/* ========================================================================== */
/*                               ENUMS                                        */
/* ========================================================================== */

enum e_type
{
	WORD = 1,
	WHITE_SPACE = ' ',
	QUOTE = '\'',
	DQUOTE = '\"',
	ENV = '$',
	PIPE_LINE = '|',
	REDIR_IN = '<',
	REDIR_OUT = '>',
	DREDIR_OUT,
	NEW_LINE = '\n',
	EXIT_STATUS,
	HERE_DOC,
	ESCAPE = '\\',
};

enum e_state
{
	IN_DQUOTE,
	IN_QUOTE,
	GENERAL
};

/* ========================================================================== */
/*                               STRUCTURES                                   */
/* ========================================================================== */

typedef struct s_expand_data
{
	char		**res;
	int			*len;
	int			*max;
	int			exit_code;
}				t_expand_data;
// extern t_env	*g_envp;

typedef struct s_cmd
{
	int				in_file;
	int				out_file;
	char			**full_cmd;
	struct s_cmd	*next;
}					t_cmd;

typedef struct s_elem
{
	char			*content;
	enum e_type		type;
	enum e_state	state;
	struct s_elem	*next;
}					t_elem;

typedef struct s_lexer
{
	char	*input;
	int		len;
	int		position;
}			t_lexer;

typedef struct s_env
{
	char			*name;
	char			*value;
	struct s_env	*next;
}					t_env;

typedef struct s_export
{
	char			*name;
	char			sep;
	char			*value;
	struct s_export	*next;
}					t_exprt;

typedef struct s_exec
{
	int			i;
	int			j;
	int			size;
	int			n_p;
	int			fork;
	int			**pe;
	char		*check;
	char		cd_path[1024];
	char		last_path[1024];
	char		old_path[1024];
	int			pos_path;
	char		**n_env;
	t_cmd		*tmp;
	t_env		*env;
	t_exprt		*export;
}				t_exec;

typedef struct s_data
{
	t_cmd			*head;
	char			*input;
	t_list			*cmd_lst;
	int				in;
	int				out;
	t_elem			*elem;
	int				error;
	int				file_error;
	int				expanded;
	char			*expnd;
	t_list			*save_error;
	struct s_env	*n_env;
}			t_data;

/* ========================================================================== */
/*                           INITIALIZATION                                   */
/* ========================================================================== */

void		init_data(t_data *data, char *input);
t_lexer		*init_lexer(char *input);
t_elem		*init_tokens(t_lexer *lexer);
void		init_env_list(char **envp);

/* ========================================================================== */
/*                              LEXER                                         */
/* ========================================================================== */

/* Core lexer functions */
int			parse_input(t_elem *token, char *input, t_lexer *lexer);
int			check_empty_line(t_data *data);

/* Token creation and manipulation */
t_elem		*create_token(char *content, enum e_type type, enum e_state state);
void		append_token(t_elem **head, t_elem *new);
t_elem		*find_last_token(t_elem *head);
void		insert_token_after(t_elem *target, t_elem *new_token);
void		remove_token(t_elem **head, t_elem *to_remove);
int			create_content_token(const char *input, int start, int end,
				t_elem **head, enum e_state state);

/* Token processing */
int			process_single_char_token(t_lexer *lexer, t_elem **head,
				char c, enum e_type type);
int			process_special_chars(t_lexer *lexer, t_elem **head);
int			process_escape_token(t_lexer *lexer, t_elem **head);
int			process_exit_status(t_lexer *lexer, t_elem **head);
void		merge_adjacent_word_tokens(t_elem **head);

/* Input handlers */
int			handle_space(const char *input, int *i, t_elem **head);
int			handle_word(const char *input, int i, t_elem **head);
int			handle_redirections(const char *input, int i, t_elem **head);
void		handle_quote(const char *input, int *i, t_elem **head);
int			handle_env(const char *input, int *i, t_elem **head);
int			handle_pipe(const char *input, int i, t_elem **head);

/* ========================================================================== */
/*                           SYNTAX VALIDATION                               */
/* ========================================================================== */

/* Quote checking */
int			check_quotes_balance(const char *input);
int			check_unclosed_quotes_in_input(const char *input);
int			check_unclosed_quotes_in_tokens(t_elem *tokens);

/* Syntax checking main functions */
int			check_syntax(t_elem *token);
int			check_initial_syntax(t_elem *curr);
// int			is_empty(char c);
// int 		check_empty_line(t_data *data);
int			check_final_syntax(enum e_state state, t_elem *prev_significant);

/* Syntax validation helpers */
int			is_redirection(enum e_type type);
int			is_quote(enum e_type type);
int			validate_pipe(t_elem *prev_significant);
int			validate_redirection_target(t_elem *next);
int			validate_redirection(t_elem *curr);
t_elem		*skip_whitespace(t_elem *token);
int			update_quote_state(enum e_type type, enum e_state *state);
int			process_token(t_elem *curr, enum e_state *state, t_elem
				**prev_significant);

/* Error handling */
char		*get_error_token(t_elem *curr);
char		*get_sredir_error(t_elem *curr);
char		*get_dredir_error(t_elem *curr);
char		*get_redirection_symbol(enum e_type type);

/* ========================================================================== */
/*                               PARSER                                       */
/* ========================================================================== */

/* Main parsing functions */
int			parse_pipeline(t_data *data);
t_cmd		*parse_command(t_data *data, t_elem **current);
int			parse_arguments(t_data *data, t_elem **current, t_cmd *cmd);

/* Redirection handling */
int			handle_redirection_in(t_data *data, t_elem **current, t_cmd *cmd);
int			handle_redirection_out(t_data *data, t_elem **current, t_cmd *cmd);
int			handle_redirection_append(t_data *data, t_elem
				**current, t_cmd *cmd);
int			handle_heredoc(t_data *data, t_elem **current, t_cmd *cmd);
int			process_redirection(t_data *data, t_elem **current, t_cmd *cmd);

/* Parser utilities */
void		skip_whitespace_ptr(t_elem **current);
int			count_command_args(t_elem *start);
int			is_redirection_target(t_elem *elem, t_elem *start);
int			allocate_cmd_args(t_cmd *cmd, int arg_count);
int			process_word_token(t_data *data, t_elem **current, t_cmd *cmd, int *arg_index);

/* ========================================================================== */
/*                             EXPANSION                                      */
/* ========================================================================== */

/* Main expansion functions */
void		expand_tokens(t_elem *token, int exit_code);
char		*expand_merged_token(char *content, int exit_code);
char		*expand_token_content(char *content, int exit_code, int should_expand);
char		*expand_exit_status(int exit_code);
char		*extract_var_name(char *str, int start, int *end);

/* Quote handling */
char		*remove_quotes(char *content, enum e_type quote_type);
char		*remove_quotes_from_token(char *content, enum e_state state);

/* Expansion utilities */
char		*realloc_result(char *result, int *max_size, int needed);
char		*get_env_value(char *name);
int			copy_var_value(char **res, int *len, int *max, char *val);
int			handle_dollar(char *content, int *i, char **res, int *len);

/* ========================================================================== */
/*                             ENVIRONMENT                                    */
/* ========================================================================== */

/* Environment list management */
t_env		*create_env_node(const char *env);
void		add_env_back(t_env **lst, t_env *new_node);
void		free_env_list(t_env *env);

/* ========================================================================== */
/*                          UTILITY FUNCTIONS                                */
/* ========================================================================== */

/* String utilities */
char		*ft_strjoin(const char *s1, const char *s2);
char		*ft_strdup(const char *s);
char		*ft_strndup(const char *s, size_t n);
int			ft_isalpha(int c);
int			ft_isalnum(int c);

/* ========================================================================== */
/*                           MEMORY MANAGEMENT                               */
/* ========================================================================== */

/* Cleanup functions */
void		free_lexer(t_lexer *lexer);
void		free_token_list(t_elem *head);
void		free_cmd_list(t_cmd *head);
void		free_cmd(t_cmd *cmd);
void		cleanup_resources(t_data *data, t_lexer *lexer, char *input);
void		cleanup_var_expansion(char *name, char *value, int is_special);

/* ========================================================================== */
/*                              DEBUGGING                                     */
/* ========================================================================== */

/* Debug printing functions */
void		print_tokens(t_elem *head);
void		print_cmd_debug(t_cmd *cmd, int cmd_num);
void		print_pipeline_debug(t_data *data);
void		print_pipeline_compact(t_data *data);
int			ft_strcmp(const char *s1, const char *s2);

/* ========================================================================== */
/*                          MAIN PROCESSING                                  */
/* ========================================================================== */

/* Main input processing */
int			process_input(char *input, int *last_exit_code);


// new expanding shit 
int		handle_special_var(char *name, int exit_code, char **value);
char	*realloc_result(char *result, int *max_size, int needed);
int		process_dollar_expansion(char *content, int *i, t_expand_data *data);
void	handle_quoted_token(t_elem *curr, int exit_code);
char	*remove_quotes(char *content, enum e_type quote_type);
char	*expand_exit_status(int exit_code);
char	*extract_var_name(char *str, int start, int *end);
int		copy_var_value(char **res, int *len, int *max, char *val);
int		is_valid_var_char(char c);
char	*get_env_value(char *name);
void	expand_tokens(t_elem *token, int exit_code);
void	handle_word_token(t_elem *curr, int exit_code);
char	*expand_token_content(char *content, int exit_code, int should_expand);
int		process_expansion_loop(char *content, t_expand_data *data);
int		process_regular_char(char *content, int *i, t_expand_data *data);


/* ===================== ENV ===================== */
extern t_env *g_envp;

t_env	*create_env_node(const char *env);
void	add_env_back(t_env **lst, t_env *new_node);
void	init_env_list(char **envp);
void	free_env_list(t_env *env);

/* ===================== SIGNALS ===================== */
void	handle_signals(void);

/* ===================== PARSER / LEXER ===================== */
int		parse_pipeline(t_data *data);
void	skip_whitespace_ptr(t_elem **current);

/* ===================== REDIRECTIONS ===================== */
int		handle_redirection_in(t_data *data, t_elem **current, t_cmd *cmd);
int		handle_redirection_out(t_data *data, t_elem **current, t_cmd *cmd);
int		handle_redirection_append(t_data *data, t_elem **current, t_cmd *cmd);
int		handle_heredoc(t_data *data, t_elem **current, t_cmd *cmd);

/* ===================== EXECUTION ===================== */
int		is_builtin(char *cmd);
int		exec_builtin(t_cmd *cmd);
int		execute_pipeline(t_data *data);

/* ===================== BUILTINS ===================== */
int		builtin_cd(char **args);
int		builtin_echo(char **args);
int		builtin_pwd(void);
int		builtin_env(void);
int		builtin_export(char **args);
int		builtin_unset(char **args);
int		builtin_exit(char **args);

/* ===================== CLEANUP ===================== */
void	free_cmd_list(t_cmd *head);
void	free_cmd(t_cmd *cmd);
// void	cleanup_resources(t_data *data, void *a, void *b);

/* ===================== DEBUG ===================== */
void	print_cmd_debug(t_cmd *cmd, int num);
void	print_pipeline_debug(t_data *data);
void 	free_str_array(char **arr);



char 	*ft_strjoin3(const char *s1, const char *s2, const char *s3);
char	*ft_strndup(const char *s, size_t n);
int		ft_strcmp(const char *s1, const char *s2);
#endif
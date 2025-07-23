#include "minishell.h"


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
# include "get_next_line.h"

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


void	free_cmd(t_cmd *cmd)
{
	int	i;

	if (!cmd)
		return ;
	if (cmd->full_cmd)
	{
		i = 0;
		while (cmd->full_cmd[i])
		{
			free(cmd->full_cmd[i]);
			i++;
		}
		free(cmd->full_cmd);
	}
	if (cmd->in_file != -1 && cmd->in_file != STDIN_FILENO)
		close(cmd->in_file);
	if (cmd->out_file != -1 && cmd->out_file != STDOUT_FILENO)
		close(cmd->out_file);
	free(cmd);
}

void	free_cmd_list(t_cmd *head)
{
	t_cmd	*current;
	t_cmd	*next;

	current = head;
	while (current)
	{
		next = current->next;
		free_cmd(current);
		current = next;
	}
}

void	free_token_list(t_elem *head)
{
	t_elem	*current;
	t_elem	*next;

	current = head;
	while (current)
	{
		next = current->next;
		if (current->content)
			free(current->content);
		free(current);
		current = next;
	}
}

void	free_lexer(t_lexer *lexer)
{
	if (lexer)
	{
		free(lexer);
	}
}

// void	cleanup_var_expansion(char *name, char *value, int is_special)
// {
// 	if (is_special)
// 		free(value);
// 	free(name);
// }

/* ========================================================================== */
/*                           MAIN PARSING FUNCTION                           */
/* ========================================================================== */

int	parse_input(t_elem *token, char *input, t_lexer *lexer)
{
	(void)lexer;
	
	if (!check_unclosed_quotes_in_input(input))
		return (0);
	if (!check_syntax(token))
		return (0);
	print_tokens(token);
	return (1);
}

/* ========================================================================== */
/*                           UTILITY FUNCTIONS                               */
/* ========================================================================== */

int	is_quote(enum e_type type)
{
	return (type == QUOTE || type == DQUOTE);
}

int	is_redirection(enum e_type type)
{
	return (type == REDIR_IN || type == REDIR_OUT || 
			type == DREDIR_OUT || type == HERE_DOC);
}

int	is_empty(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || 
			c == '\r' || c == '\f' || c == '\v');
}

t_elem	*skip_whitespace(t_elem *token)
{
	while (token && token->type == WHITE_SPACE)
		token = token->next;
	return (token);
}

/* ========================================================================== */
/*                           STRING UTILITIES                                */
/* ========================================================================== */

char	*ft_strncpy(char *dest, const char *src, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && src[i])
	{
		dest[i] = src[i];
		i++;
	}
	while (i < n)
	{
		dest[i] = '\0';
		i++;
	}
	return (dest);
}

char	*ft_strndup(const char *s, size_t n)
{
	char	*dup;

	dup = malloc(n + 1);
	if (!dup)
		return (NULL);
	ft_strncpy(dup, s, n);
	dup[n] = '\0';
	return (dup);
}

int	check_empty_line(t_data *data)
{
	int	i;

	i = 0;
	while (data->input[i] && is_empty(data->input[i]))
		i++;
	if (i == (int)ft_strlen(data->input))
	{
		free(data->input);
		return (1);
	}
	return (0);
}

/* ========================================================================== */
/*                           QUOTE HANDLING                                  */
/* ========================================================================== */

int	update_quote_state(enum e_type type, enum e_state *state)
{
	if (type == QUOTE && *state != IN_DQUOTE)
		*state = (*state == IN_QUOTE) ? GENERAL : IN_QUOTE;
	else if (type == DQUOTE && *state != IN_QUOTE)
		*state = (*state == IN_DQUOTE) ? GENERAL : IN_DQUOTE;
	return (1);
}

int	check_unclosed_quotes_in_input(const char *input)
{
	int		i;
	char	in_quote;

	i = 0;
	in_quote = 0;
	while (input[i])
	{
		if (input[i] == '\'' && in_quote != '"')
		{
			if (in_quote == '\'')
				in_quote = 0;
			else
				in_quote = '\'';
		}
		else if (input[i] == '"' && in_quote != '\'')
		{
			if (in_quote == '"')
				in_quote = 0;
			else
				in_quote = '"';
		}
		i++;
	}
	if (in_quote != 0)
	{
		ft_putstr_fd("minishell: syntax error: unclosed quote\n", 2);
		return (0);
	}
	return (1);
}

/* ========================================================================== */
/*                           ERROR MESSAGES                                  */
/* ========================================================================== */

char	*get_redirection_symbol(enum e_type type)
{
	if (type == REDIR_IN)
		return ("<");
	else if (type == REDIR_OUT)
		return (">");
	else if (type == DREDIR_OUT)
		return (">>");
	else if (type == HERE_DOC)
		return ("<<");
	return ("unknown");
}

char	*get_sredir_error(t_elem *curr)
{
	t_elem	*next;
	t_elem	*third;

	next = skip_whitespace(curr->next);
	if (!next || next->type != curr->type)
		return ("newline");
	
	third = skip_whitespace(next->next);
	if (third && third->type == curr->type)
		return (curr->type == REDIR_OUT ? ">" : "<");
	return ("newline");
}

char	*get_dredir_error(t_elem *curr)
{
	t_elem	*next;

	next = skip_whitespace(curr->next);
	if (!next || !is_redirection(next->type))
		return ("newline");
	
	if (next->type == curr->type)
		return (curr->type == DREDIR_OUT ? ">>" : "<<");
	if (next->type == REDIR_OUT)
		return (">");
	if (next->type == REDIR_IN)
		return ("<");
	return ("newline");
}

char	*get_error_token(t_elem *curr)
{
	if (curr->type == REDIR_IN || curr->type == REDIR_OUT)
		return (get_sredir_error(curr));
	if (curr->type == HERE_DOC || curr->type == DREDIR_OUT)
		return (get_dredir_error(curr));
	return ("newline");
}

/* ========================================================================== */
/*                           SYNTAX VALIDATORS                               */
/* ========================================================================== */

int	validate_pipe(t_elem *prev_significant)
{
	if (!prev_significant || prev_significant->type == PIPE_LINE ||
		is_redirection(prev_significant->type))
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
		return (0);
	}
	return (1);
}

int validate_redirection(t_elem *curr) {
    t_elem *next = skip_whitespace(curr->next);
    
    if (!next) {
        printf("minishell: syntax error near unexpected token `newline'\n");
        return (0);
    }
    if (next->type == PIPE_LINE || is_redirection(next->type)) {
        printf("minishell: syntax error near unexpected token `%s'\n", 
               get_redirection_symbol(next->type));
        return (0);
    }
    return (1);  // Parsing OK (file access happens later)
}

/* ========================================================================== */
/*                           MAIN SYNTAX CHECKER                             */
/* ========================================================================== */

int check_initial_syntax(t_elem *curr)
{
    curr = skip_whitespace(curr);
    if (!curr)
        return (1);
    
    if (curr->type == PIPE_LINE) // Only pipes are invalid at start
    {
        ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
        return (0);
    }
    return (1); // Allow redirections at start (> file, << EOF, etc.)
}

int	process_token(t_elem *curr, enum e_state *state, t_elem **prev_significant)
{
	if (curr->type == WHITE_SPACE)
		return (1);
	
	update_quote_state(curr->type, state);
	
	if (*state == GENERAL)
	{
		if (curr->type == PIPE_LINE)
		{
			if (!validate_pipe(*prev_significant))
				return (0);
		}
		else if (is_redirection(curr->type))
		{
			if (!validate_redirection(curr))
				return (0);
		}
	}
	
	if (curr->type != WHITE_SPACE)
		*prev_significant = curr;
	
	return (1);
}

int	check_final_syntax(enum e_state state, t_elem *prev_significant)
{
	if (state != GENERAL)
	{
		if (state == IN_QUOTE)
			ft_putstr_fd("minishell: unexpected EOF while looking for matching `''\n", 2);
		else
			ft_putstr_fd("minishell: unexpected EOF while looking for matching `\"'\n", 2);
		return (0);
	}
	
	 if (prev_significant && prev_significant->type == PIPE_LINE)
    {
        ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
        return (0);
    }
	return (1);
}

int	check_syntax(t_elem *token)
{
	t_elem			*curr;
	t_elem			*prev_significant;
	enum e_state	state;

	curr = token;
	prev_significant = NULL;
	state = GENERAL;
	
	if (!check_initial_syntax(curr))
		return (0);
	
	while (curr)
	{
		if (!process_token(curr, &state, &prev_significant))
			return (0);
		curr = curr->next;
	}
	
	return (check_final_syntax(state, prev_significant));
}

/* ========================================================================== */
/*                           DEBUG FUNCTIONS                                 */
/* ========================================================================== */

const char	*get_type_str(enum e_type type)
{
	switch (type)
	{
		case WORD: return ("WORD");
		case WHITE_SPACE: return ("WHITE_SPACE");
		case REDIR_IN: return ("REDIR_IN");
		case REDIR_OUT: return ("REDIR_OUT");
		case DREDIR_OUT: return ("DREDIR_OUT");
		case HERE_DOC: return ("HERE_DOC");
		case PIPE_LINE: return ("PIPE_LINE");
		case QUOTE: return ("QUOTE");
		case ENV: return ("ENV");
		case DQUOTE: return ("DQUOTE");
		case EXIT_STATUS: return ("EXIT_STATUS");
		default: return ("UNKNOWN");
	}
}

const char	*get_state_str(enum e_state state)
{
	switch (state)
	{
		case GENERAL: return ("GENERAL");
		case IN_QUOTE: return ("IN_QUOTE");
		case IN_DQUOTE: return ("IN_DQUOTE");
		default: return ("UNKNOWN_STATE");
	}
}

void	print_tokens(t_elem *head)
{
	while (head)
	{
		printf("Content: %-15s | Type: %-12s | State: %-10s\n", 
			head->content, get_type_str(head->type), get_state_str(head->state));
		head = head->next;
	}
}

// lexer 

void	init_data(t_data *data, char *input)
{
	data->input = input;
	data->head = NULL;
	data->cmd_lst = NULL;
	data->in = 0;
	data->out = 1;
	data->elem = NULL;
	data->error = 0;
	data->file_error = 0;
	data->expanded = 0;
	data->expnd = NULL;
	data->save_error = NULL;
	data->n_env = NULL;
}

t_lexer	*init_lexer(char *input)
{
	t_lexer	*lexer;

	lexer = malloc(sizeof(t_lexer));
	if (!lexer)
		return (NULL);
	lexer->input = input;
	lexer->len = ft_strlen(input);
	lexer->position = 0;
	return (lexer);
}

t_elem	*init_tokens(t_lexer *lexer)
{
	t_elem	*head;
	char	current;

	head = NULL;
	while (lexer->position < lexer->len)
	{
		current = lexer->input[lexer->position];
		if (current == ' ' || current == '\t')
			lexer->position = handle_space(lexer->input, &(lexer->position), &head);
		else if (current == '\'' || current == '\"')
			handle_quote(lexer->input, &(lexer->position), &head);
		else if (current == '>' || current == '<')
			lexer->position = handle_redirections(lexer->input, lexer->position, &head);
		else if (process_special_chars(lexer, &head))
			continue ;
		else
			lexer->position = handle_word(lexer->input, lexer->position, &head);
		if (lexer->position == -1)
		{
			free_token_list(head);
			return (NULL);
		}
	}
	return (head);
}

void	append_token(t_elem **head, t_elem *new)
{
	t_elem	*tmp;

	if (!head || !new)
		return ;
	if (!*head)
	{
		*head = new;
		return ;
	}
	tmp = *head;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new;
}

t_elem	*create_token(char *content, enum e_type type, enum e_state state)
{
	t_elem	*token;

	if (!content)
		return (NULL);
	token = malloc(sizeof(t_elem));
	if (!token)
		return (NULL);
	token->content = ft_strdup(content);
	if (!token->content)
	{
		free(token);
		return (NULL);
	}
	token->type = type;
	token->state = state;
	token->next = NULL;
	return (token);
}

int	process_single_char_token(t_lexer *lexer, t_elem **head, char c, enum e_type type)
{
	char	*content;
	t_elem	*token;

	(void)lexer;
	content = ft_strndup(&c, 1);
	if (!content)
		return (0);
	token = create_token(content, type, GENERAL);
	free(content);
	if (!token)
		return (0);
	append_token(head, token);
	return (1);
}

int	process_special_chars(t_lexer *lexer, t_elem **head)
{
	char	current;

	current = lexer->input[lexer->position];
	if (current == '$' && lexer->input[lexer->position + 1] == '?')
		return (process_exit_status(lexer, head));
	// else if (current == '$')
	// {
	// 	lexer->position = handle_env(lexer->input, &(lexer->position), head);
	// 	return (lexer->position != -1);
	// }
	else if (current == '|')
	{
		if (!process_single_char_token(lexer, head, current, PIPE_LINE))
			return (0);
		lexer->position++;
		return (1);
	}
	else if (current == '\\')
		return (process_escape_token(lexer, head));
	else if (current == '\n')
	{
		if (!process_single_char_token(lexer, head, current, NEW_LINE))
			return (0);
		lexer->position++;
		return (1);
	}
	return (0);
}

int	process_exit_status(t_lexer *lexer, t_elem **head)
{
	char	*content;
	t_elem	*token;

	content = ft_strndup("$?", 2);
	if (!content)
		return (0);
	token = create_token(content, EXIT_STATUS, GENERAL);
	free(content);
	if (!token)
		return (0);
	append_token(head, token);
	lexer->position += 2;
	return (1);
}

int	process_escape_token(t_lexer *lexer, t_elem **head)
{
	char	*content;
	t_elem	*token;

	if (!lexer->input[lexer->position + 1])
		return (0);
	content = ft_strndup(&lexer->input[lexer->position], 2);
	if (!content)
		return (0);
	token = create_token(content, ESCAPE, GENERAL);
	free(content);
	if (!token)
		return (0);
	append_token(head, token);
	lexer->position += 2;
	return (1);
}

void	handle_quote(const char *input, int *i, t_elem **head)
{
	char			quote;
	enum e_state	state;
	int				start;

	quote = input[(*i)++];
	state = (quote == '\'') ? IN_QUOTE : IN_DQUOTE;
	start = *i;
	while (input[*i] && input[*i] != quote)
		(*i)++;
	if (*i == start)
	{
		if (input[*i] == quote)
			(*i)++;
		return ;
	}
	if (!create_content_token(input, start, *i, head, state))
		return ;
	if (input[*i] == quote)
		(*i)++;
}

int	handle_redirections(const char *input, int i, t_elem **head)
{
	enum e_type	type;
	int			start;
	char		*content;
	t_elem		*token;

	start = i;
	if (input[i] == '>' && input[i + 1] && input[i + 1] == '>')
	{
		type = DREDIR_OUT;
		i += 2;
	}
	else if (input[i] == '<' && input[i + 1] && input[i + 1] == '<')
	{
		type = HERE_DOC;
		i += 2;
	}
	else if (input[i] == '>')
	{
		type = REDIR_OUT;
		i++;
	}
	else
	{
		type = REDIR_IN;
		i++;
	}
	content = ft_strndup(input + start, i - start);
	if (!content)
		return (-1);
	token = create_token(content, type, GENERAL);
	free(content);
	if (!token)
		return (-1);
	append_token(head, token);
	return (i);
}

static int	create_env_word_token(const char *input, int start, int end,
								t_elem **head, enum e_type type)
{
	char	*content;
	t_elem	*token;

	content = ft_strndup(&input[start], end - start);
	if (!content)
		return (0);
	token = create_token(content, type, GENERAL);
	free(content);
	if (!token)
		return (0);
	append_token(head, token);
	return (1);
}

int	handle_env(const char *input, int *i, t_elem **head)
{
	int	start;

	start = *i;
	if (input[*i] != '$')
		return (*i);
	(*i)++;
	if (input[*i] == '\0' || input[*i] == ' ')
	{
		if (!create_env_word_token("$", 0, 1, head, WORD))
			return (-1);
		return (*i);
	}
	if (ft_isalpha(input[*i]) || input[*i] == '_')
	{
		while (ft_isalnum(input[*i]) || input[*i] == '_')
			(*i)++;
		if (!create_env_word_token(input, start, *i, head, ENV))
			return (-1);
		return (*i);
	}
	if (!create_env_word_token("$", 0, 1, head, WORD))
		return (-1);
	return (*i);
}

int	create_content_token(const char *input, int start, int end,
							t_elem **head, enum e_state state)
{
	char	*content;
	t_elem	*token;

	if (end <= start)
		return (1);
	content = ft_strndup(input + start, end - start);
	if (!content)
		return (0);
	token = create_token(content, WORD, state);
	free(content);
	if (!token)
		return (0);
	append_token(head, token);
	return (1);
}

int	handle_space(const char *input, int *i, t_elem **head)
{
	int		start;
	char	*space;
	t_elem	*token;

	start = *i;
	while (input[*i] == ' ' || input[*i] == '\t')
		(*i)++;
	space = ft_strndup(input + start, *i - start);
	if (!space)
		return (-1);
	token = create_token(space, WHITE_SPACE, GENERAL);
	free(space);
	if (!token)
		return (-1);
	append_token(head, token);
	return (*i);
}
int	handle_word(const char *input, int i, t_elem **head)
{
	int		start;
	char	*content;
	t_elem	*token;

	start = i;
	while (input[i] && input[i] != ' ' && input[i] != '\t' &&
		input[i] != '|' && input[i] != '<' && input[i] != '>' &&
		input[i] != '\'' && input[i] != '\"')
		i++;
	if (i > start)
	{
		content = ft_strndup(input + start, i - start);
		if (!content)
			return (-1);
		token = create_token(content, WORD, GENERAL);
		free(content);
		if (!token)
			return (-1);
		append_token(head, token);
	}
	return (i);
}

/* minishell/lexer_merge.c */
void    merge_adjacent_word_tokens(t_elem **head)
{
    t_elem  *curr;
    t_elem  *next;
    char    *merged;

    curr = *head;
    while (curr && curr->next)
    {
        next = curr->next;
        /* ――― merge only when BOTH tokens have the **same** quote context ――― */
        if (curr->type == WORD && next->type == WORD &&
            curr->state == next->state)                /*  ✅ key change  */
        {
            merged = ft_strjoin(curr->content, next->content);
            if (!merged)
                return ;
            free(curr->content);
            curr->content = merged;
            curr->next = next->next;
            free(next->content);
            free(next);
            continue;          /* stay on current node in case there’s another WORD */
        }
        curr = curr->next;
    }
}
// parser

static int	create_heredoc_pipe(int pipe_fd[2])
{
	if (pipe(pipe_fd) == -1)
	{
		perror("minishell: pipe");
		return (0);
	}
	return (1);
}

static void	write_to_pipe(int pipe_fd, const char *line)
{
	write(pipe_fd, line, ft_strlen(line));
	write(pipe_fd, "\n", 1);
}

static int	read_heredoc_lines(const char *delimiter, int pipe_fd)
{
	char	*line;
	int		len;

	while (1)
	{
		write(1, "> ", 2);
		line = get_next_line(STDIN_FILENO);
		if (!line)
		{
			ft_putstr_fd("minishell: warning: here-doc delimited by EOF\n", 2);
			break ;
		}
		len = ft_strlen(line);
		if (len > 0 && line[len - 1] == '\n')
			line[len - 1] = '\0';
		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		write_to_pipe(pipe_fd, line);
		free(line);
	}
	return (1);
}

int	handle_heredoc(t_data *data, t_elem **current, t_cmd *cmd)
{
	int		pipe_fd[2];
	char	*delimiter;

	(void)data;
	*current = (*current)->next;
	skip_whitespace_ptr(current);
	if (!*current || (*current)->type != WORD)
		return (0);
	delimiter = (*current)->content;
	if (!create_heredoc_pipe(pipe_fd))
		return (0);
	if (!read_heredoc_lines(delimiter, pipe_fd[1]))
	{
		close(pipe_fd[1]);
		return (0);
	}
	close(pipe_fd[1]);
	if (cmd->in_file != STDIN_FILENO)
		close(cmd->in_file);
	cmd->in_file = pipe_fd[0];
	*current = (*current)->next;
	return (1);
}


int handle_redirection_in(t_data *data, t_elem **current, t_cmd *cmd)
{
    int fd;

    if (!data || !current || !*current || !cmd)
        return (0);

    *current = (*current)->next;
    skip_whitespace_ptr(current);

    if (!*current || (*current)->type != WORD)
        return (0);

    fd = open((*current)->content, O_RDONLY);
    if (fd == -1)
    {
        perror((*current)->content);
        data->file_error = 1;
        *current = (*current)->next;
        return (0);
    }

    if (cmd->in_file != STDIN_FILENO)
        close(cmd->in_file);
    cmd->in_file = fd;

    *current = (*current)->next;
    return (1);
}

// Handles '>' redirection (truncate output)
int handle_redirection_out(t_data *data, t_elem **current, t_cmd *cmd)
{
    int fd;

    if (!data || !current || !*current || !cmd)
        return (0);

    *current = (*current)->next;
    skip_whitespace_ptr(current);

    if (!*current || (*current)->type != WORD)
        return (0);

    fd = open((*current)->content, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror((*current)->content);
        data->file_error = 1;
        *current = (*current)->next;
        return (0);
    }

    if (cmd->out_file != STDOUT_FILENO)
        close(cmd->out_file);
    cmd->out_file = fd;

    *current = (*current)->next;
    return (1);
}

// Handles '>>' redirection (append to output)
int handle_redirection_append(t_data *data, t_elem **current, t_cmd *cmd)
{
    int fd;

    if (!data || !current || !*current || !cmd)
        return (0);

    *current = (*current)->next;
    skip_whitespace_ptr(current);

    if (!*current || (*current)->type != WORD)
        return (0);

    fd = open((*current)->content, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        perror((*current)->content);
        data->file_error = 1;
        *current = (*current)->next;
        return (0);
    }

    if (cmd->out_file != STDOUT_FILENO)
        close(cmd->out_file);
    cmd->out_file = fd;

    *current = (*current)->next;
    return (1);
}


void	skip_whitespace_ptr(t_elem **current)
{
	if (!current)
		return;
	while (*current && (*current)->type == WHITE_SPACE)
		*current = (*current)->next;
}

int	count_command_args(t_elem *start)
{
	int	count;
	t_elem	*current;

	count = 0;
	current = start;
	while (current && current->type != PIPE_LINE)
	{
		if (current->type == WORD || current->type == ENV)
		{
			if (!is_redirection_target(current, start))
				count++;
		}
		current = current->next;
	}
	return (count);
}

// Fixed memory allocation with proper cleanup
int	allocate_cmd_args(t_cmd *cmd, int arg_count)
{
	if (!cmd)
		return (0);
	if (arg_count <= 0)
	{
		cmd->full_cmd = NULL;
		return (1);
	}
	cmd->full_cmd = malloc(sizeof(char *) * (arg_count + 1));
	if (!cmd->full_cmd)
		return (0);
	ft_memset(cmd->full_cmd, 0, sizeof(char *) * (arg_count + 1));
	return (1);
}


int	process_word_token(t_data *data, t_elem **current, t_cmd *cmd, int *arg_index)
{
	if (!data || !current || !*current || !cmd || !arg_index)
		return (0);
	if (cmd->full_cmd && !is_redirection_target(*current, data->elem))
	{
		cmd->full_cmd[*arg_index] = ft_strdup((*current)->content);
		if (!cmd->full_cmd[*arg_index])
			return (0);
		(*arg_index)++;
	}
	*current = (*current)->next;
	return (1);
}

int	is_redirection_target(t_elem *elem, t_elem *start)
{
	t_elem	*current;
	t_elem	*prev;

	if (!elem || !start)
		return (0);
	current = start;
	prev = NULL;
	while (current)
	{
		if (current == elem)
			break;
		if (current->type != WHITE_SPACE)
			prev = current;
		current = current->next;
	}
	if (!current || !prev)
		return (0);
	return (prev->type == REDIR_IN || prev->type == REDIR_OUT ||
		prev->type == DREDIR_OUT || prev->type == HERE_DOC);
}



int	parse_pipeline(t_data *data)
{
	t_elem	*current;
	t_cmd	*current_cmd;
	t_cmd	*last_cmd;

	if (!data || !data->elem)
		return (0);
	current = data->elem;
	data->head = NULL;
	last_cmd = NULL;
	while (current)
	{
		skip_whitespace_ptr(&current);
		if (!current)
			break;
		if (current->type == PIPE_LINE)
		{
			current = current->next;
			skip_whitespace_ptr(&current);
			if (!current)
				return (0);
			continue;
		}
		current_cmd = parse_command(data, &current);
		if (!current_cmd)
		{
			free_cmd_list(data->head);
			return (0);
		}
		if (!data->head)
			data->head = current_cmd;
		else
			last_cmd->next = current_cmd;
		last_cmd = current_cmd;
	}
	return (data->head != NULL);
}

// Fixed memory cleanup on failure
t_cmd	*parse_command(t_data *data, t_elem **current)
{
	t_cmd	*cmd;

	if (!data || !current)
		return (NULL);
	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->in_file = STDIN_FILENO;
	cmd->out_file = STDOUT_FILENO;
	cmd->full_cmd = NULL;
	cmd->next = NULL;
	if (!parse_arguments(data, current, cmd))
	{
		free_cmd(cmd);
		return (NULL);
	}
	return (cmd);
}

// Fixed argument parsing with better error handling
int	parse_arguments(t_data *data, t_elem **current, t_cmd *cmd)
{
	int	arg_count;
	int	arg_index;

	if (!data || !current || !cmd)
		return (0);
	arg_count = count_command_args(*current);
	if (!allocate_cmd_args(cmd, arg_count))
		return (0);
	arg_index = 0;
	while (*current && (*current)->type != PIPE_LINE)
	{
		skip_whitespace_ptr(current);
		if (!*current || (*current)->type == PIPE_LINE)
			break;
		// Handle both WORD and ENV tokens as arguments
		// 
		else if ((*current)->type == WORD || (*current)->type == ENV)
{
	char *merged = ft_strdup((*current)->content);
	if (!merged)
		return (0);

	t_elem *start = *current;
	t_elem *next = (*current)->next;

	while (next && (next->type == WORD || next->type == ENV))
	{
		// Check if there is whitespace between current and next
		t_elem *tmp = start->next;
		int separated_by_whitespace = 0;
		while (tmp && tmp != next)
		{
			if (tmp->type == WHITE_SPACE)
			{
				separated_by_whitespace = 1;
				break;
			}
			tmp = tmp->next;
		}
		if (separated_by_whitespace)
			break;

		char *tmp_str = ft_strjoin(merged, next->content);
		free(merged);
		if (!tmp_str)
			return (0);
		merged = tmp_str;

		start = next;
		next = next->next;
	}

	if (cmd->full_cmd)
		cmd->full_cmd[arg_index++] = merged;

	*current = start->next;
}

		else if (!process_redirection(data, current, cmd))
			return (0);
	}
	return (1);
}

// Simplified redirection processing
int	process_redirection(t_data *data, t_elem **current, t_cmd *cmd)
{
	if (!data || !current || !*current || !cmd)
		return (0);
	if ((*current)->type == REDIR_IN)
		return (handle_redirection_in(data, current, cmd));
	if ((*current)->type == REDIR_OUT)
		return (handle_redirection_out(data, current, cmd));
	if ((*current)->type == DREDIR_OUT)
		return (handle_redirection_append(data, current, cmd));
	if ((*current)->type == HERE_DOC)
		return (handle_heredoc(data, current, cmd));
	*current = (*current)->next;
	return (1);
}

// expande 
#include "minishell.h"



int is_valid_var_char(char c)
{
    return (ft_isalnum(c) || c == '_');
}

char *realloc_result(char *result, int *max_size, int needed)
{
    char *new_result;

    if (needed <= *max_size)
        return (result);
    
    *max_size = needed + 100;
    new_result = realloc(result, *max_size);
    if (!new_result)
    {
        free(result);
        return (NULL);
    }
    return (new_result);
}

int copy_var_value(char **res, int *len, int *max, char *val)
{
    int vlen;
    char *temp;

    if (!val)
        val = "";
    
    vlen = strlen(val);
    temp = realloc_result(*res, max, *len + vlen + 1);
    if (!temp)
        return (0);
    
    *res = temp;
    strcpy(*res + *len, val);
    *len += vlen;
    return (1);
}

char *extract_var_name(char *str, int start, int *end)
{
    int i;

    if (!str || !end)
        return (NULL);
    
    i = start;
    if (str[i] == '?')
    {
        *end = i + 1;
        return (ft_strdup("?"));
    }
    
    while (str[i] && is_valid_var_char(str[i]))
        i++;
    
    *end = i;
    if (i == start)
        return (NULL);
    
    return (ft_strndup(str + start, i - start));
}

char *expand_exit_status(int exit_code)
{
    return (ft_itoa(exit_code));
}

int handle_special_var(char *name, int exit_code, char **value)
{
    if (!name || !value)
        return (0);
    
    if (strcmp(name, "?") == 0)
    {
        *value = expand_exit_status(exit_code);
        return (1); // indicates we allocated memory for value
    }
    
    *value = get_env_value(name);
    return (0); // indicates value points to existing memory
}

// void cleanup_var_expansion(char *name, char *value, int is_special)
// {
//     if (name)
//         free(name);
//     if (value && is_special)
//         free(value);
// }

int process_regular_char(char *content, int *i, t_expand_data *data)
{
    char *temp;
    
    if (!content || !i || !data || !data->res || !data->len || !data->max)
        return (0);
    
    temp = realloc_result(*(data->res), data->max, *(data->len) + 2);
    if (!temp)
        return (0);
    
    *(data->res) = temp;
    (*(data->res))[*(data->len)] = content[*i];
    (*(data->len))++;
    (*i)++;
    
    return (1);
}

int process_dollar_expansion(char *content, int *i, t_expand_data *data)
{
    int var_end;
    char *name;
    char *value;
    int is_special;

    if (!content || !i || !data)
        return (0);

    name = extract_var_name(content, *i, &var_end);
    if (name)
    {
        is_special = handle_special_var(name, data->exit_code, &value);
        if (!copy_var_value(data->res, data->len, data->max, value))
        {
            cleanup_var_expansion(name, value, is_special);
            return (0);
        }
        cleanup_var_expansion(name, value, is_special);
        *i = var_end;
        return (1);
    }
    
    // If we couldn't extract a variable name, just copy the '$' literally
    if (!process_regular_char("$", &(int){0}, data))
        return (0);
    
    return (1);
}

int process_expansion_loop(char *content, t_expand_data *data)
{
    int i;

    if (!content || !data)
        return (0);
    
    i = 0;
    while (content[i])
    {
        if (content[i] == '$')
        {
            i++;
            if (!process_dollar_expansion(content, &i, data))
                return (0);
        }
        else if (!process_regular_char(content, &i, data))
            return (0);
    }
    return (1);
}

char *expand_token_content(char *content, int exit_code, int should_expand)
{
    char *res;
    int len;
    int max;
    t_expand_data data;

    if (!content)
        return (NULL);
    
    if (!should_expand)
        return (ft_strdup(content));
    
    len = 0;
    max = 1024;
    res = malloc(max);
    if (!res)
        return (NULL);
    
    data.res = &res;
    data.len = &len;
    data.max = &max;
    data.exit_code = exit_code;
    
    if (!process_expansion_loop(content, &data))
    {
        free(res);
        return (NULL);
    }
    
    res[len] = '\0';
    return (res);
}

void handle_word_token(t_elem *curr, int exit_code)
{
    int should_expand;
    char *exp;

    if (!curr || !curr->content)
        return;
    
    // NEVER expand inside a single-quoted token
    should_expand = (curr->state != IN_QUOTE);
    
    exp = expand_token_content(curr->content, exit_code, should_expand);
    if (exp)
    {
        free(curr->content);
        curr->content = exp;
        curr->type = WORD;
        // keep curr->state unchanged – it may still be IN_DQUOTE or GENERAL
    }
}

char *remove_quotes(char *content, enum e_type quote_type)
{
    int len;
    char quote_char;

    if (!content)
        return (NULL);
    
    len = strlen(content);
    if (len < 2)
        return (ft_strdup(content));
    
    if (quote_type == QUOTE)
        quote_char = '\'';
    else if (quote_type == DQUOTE)
        quote_char = '"';
    else
        return (ft_strdup(content));
    
    if (content[0] == quote_char && content[len - 1] == quote_char)
        return (ft_strndup(content + 1, len - 2));
    
    return (ft_strdup(content));
}

void handle_quoted_token(t_elem *curr, int exit_code)
{
    char *unquoted;
    char *expanded;
    int should_expand;

    if (!curr || !curr->content)
        return;
    
    unquoted = remove_quotes(curr->content, curr->type);
    if (!unquoted)
        return;
    
    // Only expand inside double quotes, not single quotes
    should_expand = (curr->type == DQUOTE);
    
    expanded = expand_token_content(unquoted, exit_code, should_expand);
    free(unquoted);
    
    if (expanded)
    {
        free(curr->content);
        curr->content = expanded;
        curr->type = WORD;
    }
}

void expand_tokens(t_elem *token, int exit_code)
{
    t_elem *curr;

    if (!token)
        return;
    
    curr = token;
    while (curr)
    {
        if (curr->type == QUOTE || curr->type == DQUOTE)
            handle_quoted_token(curr, exit_code);
        else if ((curr->type == WORD || curr->type == ENV) 
                 && curr->state != IN_QUOTE)
            handle_word_token(curr, exit_code);
        curr = curr->next;
    }
}

// execition

t_env	*g_envp = NULL;

char	*ft_strjoin3(const char *s1, const char *s2, const char *s3)
{
	char	*joined;

	size_t len1, len2, len3;
	if (!s1 || !s2 || !s3)
		return (NULL);
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	len3 = ft_strlen(s3);
	joined = malloc(len1 + len2 + len3 + 1);
	if (!joined)
		return (NULL);
	ft_strlcpy(joined, s1, len1 + 1);
	ft_strlcat(joined, s2, len1 + len2 + 1);
	ft_strlcat(joined, s3, len1 + len2 + len3 + 1);
	return (joined);
}

void	free_str_array(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return ;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

// ======================== ENVIRONMENT UTILS ============================ //

t_env	*create_env_node(const char *env)
{
	t_env	*node;
	char	*sep;

	if (!env)
		return (NULL);
	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	sep = strchr(env, '=');
	if (!sep)
		return (free(node), NULL);
	node->name = ft_strndup(env, sep - env);
	if (!node->name)
		return (free(node), NULL);
	node->value = strdup(sep + 1);
	if (!node->value)
		return (free(node->name), free(node), NULL);
	node->next = NULL;
	return (node);
}

void	add_env_back(t_env **lst, t_env *new_node)
{
	t_env	*temp;

	if (!lst || !new_node)
		return ;
	if (!*lst)
	{
		*lst = new_node;
		return ;
	}
	temp = *lst;
	while (temp->next)
		temp = temp->next;
	temp->next = new_node;
}

void	init_env_list(char **envp)
{
	int		i;
	t_env	*node;

	i = 0;
	if (!envp)
		return ;
	while (envp[i])
	{
		node = create_env_node(envp[i]);
		if (node)
			add_env_back(&g_envp, node);
		i++;
	}
}

void	free_env_list(t_env *env)
{
	t_env	*tmp;

	while (env)
	{
		tmp = env;
		env = env->next;
		if (tmp->name)
			free(tmp->name);
		if (tmp->value)
			free(tmp->value);
		free(tmp);
	}
}

char	*get_env_value(char *key)
{
	t_env	*tmp;

	tmp = g_envp;
	if (!key)
		return (NULL);
	while (tmp)
	{
		if (tmp->name && !ft_strcmp(tmp->name, key))
			return (tmp->value);
		tmp = tmp->next;
	}
	return (NULL);
}

char	*get_cmd_path(char *cmd)
{
	char	**paths;
	char	*env_path;
	char	*full;
	int		i;

	if (!cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	env_path = get_env_value("PATH");
	if (!env_path)
		return (ft_strdup(cmd));
	paths = ft_split(env_path, ':');
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		full = ft_strjoin3(paths[i], "/", cmd);
		if (full && access(full, X_OK) == 0)
		{
			free_str_array(paths);
			return (full);
		}
		if (full)
			free(full);
		i++;
	}
	free_str_array(paths);
	return (NULL);
}

char	**env_to_array(t_env *env)
{
	int		count = 0, i;
	char	**arr;
	char	*tmp;
	t_env	*copy;

	count = 0, i = 0;
	copy = env;
	// Count environment variables
	while (copy)
	{
		if (copy->name && copy->value) // FIXED: Only count valid entries
			count++;
		copy = copy->next;
	}
	arr = malloc(sizeof(char *) * (count + 1));
	if (!arr)
		return (NULL);
	while (env && i < count)
	{
		if (env->name && env->value)
		{
			tmp = ft_strjoin(env->name, "=");
			if (tmp)
			{
				arr[i] = ft_strjoin(tmp, env->value);
				free(tmp);
				if (arr[i])
					i++;
				else
				{
					// FIXED: Clean up on failure
					while (--i >= 0)
						free(arr[i]);
					free(arr);
					return (NULL);
				}
			}
		}
		env = env->next;
	}
	arr[i] = NULL;
	return (arr);
}

// ======================== BUILT-IN COMMANDS ============================ //

int	builtin_pwd(void)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)))
	{
		printf("%s\n", cwd);
		return (0);
	}
	perror("pwd");
	return (1);
}

int	builtin_echo(char **args)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	if (!args || !args[0]) // FIXED: Better null checking
		return (1);
	if (args[1] && ft_strcmp(args[1], "-n") == 0)
	{
		newline = 0;
		i++;
	}
	while (args[i])
	{
		printf("%s", args[i]);
		if (args[i + 1])
			printf(" ");
		i++;
	}
	if (newline)
		printf("\n");
	return (0);
}

int	builtin_exit(char **args)
{
	int	code;

	code = 0;
	if (args && args[1])
		code = ft_atoi(args[1]);
	// FIXED: Clean up before exit
	free_env_list(g_envp);
	exit(code);
}

int	builtin_env(void)
{
	t_env	*tmp;

	tmp = g_envp;
	while (tmp)
	{
		if (tmp->name && tmp->value)
			printf("%s=%s\n", tmp->name, tmp->value);
		tmp = tmp->next;
	}
	return (0);
}

/*the export logic */
/* ---------------------- DISPLAY FUNCTIONS ---------------------- */

static void print_exported_vars(void)
{
    t_env *tmp = g_envp;
    
    while (tmp)
    {
        if (tmp->value)
            printf("declare -x %s=\"%s\"\n", tmp->name, tmp->value);
        else
            printf("declare -x %s\n", tmp->name);
        tmp = tmp->next;
    }
}

/* ---------------------- VARIABLE UPDATE FUNCTIONS ---------------------- */

static int update_existing_var(t_env *node, char *new_value)
{
    if (!node || !new_value)
        return (0);
    
    if (node->value)
        free(node->value);
    node->value = ft_strdup(new_value);
    return (1);
}

static int create_new_var(char *name, char *value)
{
    t_env *new_node;
    char *full_var;

    if (!name || !value)
        return (0);
    
    full_var = ft_strjoin3(name, "=", value);
    if (!full_var)
        return (0);
    
    new_node = create_env_node(full_var);
    free(full_var);
    if (!new_node)
        return (0);
    
    add_env_back(&g_envp, new_node);
    return (1);
}

/* ---------------------- ARGUMENT PROCESSING ---------------------- */

static int process_var_assignment(char *arg)
{
    char *sep = ft_strchr(arg, '=');
    t_env *tmp;

    if (!sep)
        return (0);
    
    *sep = '\0';  // Temporarily split at '='
    tmp = g_envp;
    
    // Try to update existing variable
    while (tmp)
    {
        if (tmp->name && !ft_strcmp(tmp->name, arg))
        {
            *sep = '=';
            return update_existing_var(tmp, sep + 1);
        }
        tmp = tmp->next;
    }
    
    // Or create new one
    *sep = '=';  // Restore original string
    return create_new_var(arg, sep + 1);
}

/* ---------------------- MAIN EXPORT FUNCTION ---------------------- */

int builtin_export(char **args)
{
    if (!args || !args[0])
        return (1);
    
    if (!args[1])
    {
        print_exported_vars();
        return (0);
    }

    int i = 1;
    while (args[i])
    {
        if (!process_var_assignment(args[i]))
            return (1);
        i++;
    }
    return (0);
}
/*the end of it */
int	builtin_unset(char **args)
{
	int	i;

	i = 1;
	t_env *tmp, *prev;
	if (!args || !args[0]) // FIXED: Better null checking
		return (1);
	while (args[i])
	{
		tmp = g_envp;
		prev = NULL;
		while (tmp)
		{
			if (tmp->name && !ft_strcmp(tmp->name, args[i]))
			{
				if (prev)
					prev->next = tmp->next;
				else
					g_envp = tmp->next;
				if (tmp->name)
					free(tmp->name);
				if (tmp->value)
					free(tmp->value);
				free(tmp);
				break ;
			}
			prev = tmp;
			tmp = tmp->next;
		}
		i++;
	}
	return (0);
}

int	builtin_cd(char **args)
{
	char	*home;

	if (!args || !args[0]) // FIXED: Better null checking
		return (1);
	if (!args[1])
	{
		home = get_env_value("HOME");
		if (!home)
			return (1);
		if (chdir(home) != 0)
		{
			perror("cd");
			return (1);
		}
	}
	else if (chdir(args[1]) != 0)
	{
		perror("cd");
		return (1);
	}
	return (0);
}

int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	return (!ft_strcmp(cmd, "cd") || !ft_strcmp(cmd, "echo") || !ft_strcmp(cmd,
			"pwd") || !ft_strcmp(cmd, "export") || !ft_strcmp(cmd, "unset")
		|| !ft_strcmp(cmd, "env") || !ft_strcmp(cmd, "exit"));
}

int	exec_builtin(t_cmd *cmd)
{
	if (!cmd || !cmd->full_cmd || !cmd->full_cmd[0])
		return (1);
	if (!ft_strcmp(cmd->full_cmd[0], "echo"))
		return (builtin_echo(cmd->full_cmd));
	if (!ft_strcmp(cmd->full_cmd[0], "cd"))
		return (builtin_cd(cmd->full_cmd));
	if (!ft_strcmp(cmd->full_cmd[0], "pwd"))
		return (builtin_pwd());
	if (!ft_strcmp(cmd->full_cmd[0], "export"))
		return (builtin_export(cmd->full_cmd));
	if (!ft_strcmp(cmd->full_cmd[0], "unset"))
		return (builtin_unset(cmd->full_cmd));
	if (!ft_strcmp(cmd->full_cmd[0], "env"))
		return (builtin_env());
	if (!ft_strcmp(cmd->full_cmd[0], "exit"))
		return (builtin_exit(cmd->full_cmd));
	return (1);
}

// FIXED: Handle single commands (no pipeline) separately
int	execute_single_command(t_cmd *cmd)
{
	char	*path;
	char	**envp;
	pid_t	pid;
	int		status;
	int		saved_stdin = -1, saved_stdout;
	int		ret;

	if (!cmd || !cmd->full_cmd || !cmd->full_cmd[0])
		return (1);
	// Handle built-ins in parent process for single commands
	if (is_builtin(cmd->full_cmd[0]))
	{
		// Set up redirections if needed
		saved_stdin = -1, saved_stdout = -1;
		if (cmd->in_file != STDIN_FILENO)
		{
			saved_stdin = dup(STDIN_FILENO);
			dup2(cmd->in_file, STDIN_FILENO);
		}
		if (cmd->out_file != STDOUT_FILENO)
		{
			saved_stdout = dup(STDOUT_FILENO);
			dup2(cmd->out_file, STDOUT_FILENO);
		}
		ret = exec_builtin(cmd);
		// Restore original file descriptors
		if (saved_stdin != -1)
		{
			dup2(saved_stdin, STDIN_FILENO);
			close(saved_stdin);
		}
		if (saved_stdout != -1)
		{
			dup2(saved_stdout, STDOUT_FILENO);
			close(saved_stdout);
		}
		return (ret);
	}
	// Handle external commands
	envp = env_to_array(g_envp);
	if (!envp)
		return (1);
	pid = fork();
	if (pid == 0)
	{
		// Child process
		if (cmd->in_file != STDIN_FILENO)
		{
			dup2(cmd->in_file, STDIN_FILENO);
			close(cmd->in_file);
		}
		if (cmd->out_file != STDOUT_FILENO)
		{
			dup2(cmd->out_file, STDOUT_FILENO);
			close(cmd->out_file);
		}
		path = get_cmd_path(cmd->full_cmd[0]);
		if (!path)
		{
			fprintf(stderr, "%s: command not found\n", cmd->full_cmd[0]);
			free_str_array(envp);
			exit(127);
		}
		execve(path, cmd->full_cmd, envp);
		perror(path);
		free(path);
		free_str_array(envp);
		exit(127);
	}
	else if (pid < 0)
	{
		perror("fork");
		free_str_array(envp);
		return (1);
	}
	// Parent process - wait for child
	waitpid(pid, &status, 0);
	free_str_array(envp);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (1);
}

int	execute_pipeline(t_data *data)
{
	t_cmd	*cmd;
	int		pipefd[2];
	int		prev_fd;
	pid_t	pid;
	char	*path;
	char	**envp;
	int		cmd_count;
	int		ret;

	prev_fd = -1;
	cmd_count = 0;
	if (!data || !data->head)
		return (1);
	// FIXED: Count commands and handle single command case
	cmd = data->head;
	while (cmd)
	{
		if (cmd->full_cmd && cmd->full_cmd[0])
			cmd_count++;
		cmd = cmd->next;
	}
	if (cmd_count == 1)
		return (execute_single_command(data->head));
	// Handle pipeline
	cmd = data->head;
	envp = env_to_array(g_envp);
	if (!envp)
		return (1);
	while (cmd)
	{
		// Check if command is valid
		if (!cmd->full_cmd || !cmd->full_cmd[0])
		{
			cmd = cmd->next;
			continue ;
		}
		// FIXED: Only create pipe if there's a next command
		if (cmd->next && pipe(pipefd) < 0)
		{
			perror("pipe");
			free_str_array(envp);
			if (prev_fd != -1)
				close(prev_fd);
			return (1);
		}
		pid = fork();
		if (pid == 0)
		{
			// Child process
			if (cmd->in_file != STDIN_FILENO)
			{
				dup2(cmd->in_file, STDIN_FILENO);
				close(cmd->in_file);
			}
			else if (prev_fd != -1)
			{
				dup2(prev_fd, STDIN_FILENO);
				close(prev_fd);
			}
			if (cmd->out_file != STDOUT_FILENO)
			{
				dup2(cmd->out_file, STDOUT_FILENO);
				close(cmd->out_file);
			}
			else if (cmd->next)
			{
				dup2(pipefd[1], STDOUT_FILENO);
				close(pipefd[1]);
			}
			// FIXED: Close all pipe file descriptors in child
			if (cmd->next)
				close(pipefd[0]);
			if (is_builtin(cmd->full_cmd[0]))
			{
				ret = exec_builtin(cmd);
				free_str_array(envp);
				exit(ret);
			}
			path = get_cmd_path(cmd->full_cmd[0]);
			if (!path)
			{
				fprintf(stderr, "%s: command not found\n", cmd->full_cmd[0]);
				free_str_array(envp);
				exit(127);
			}
			execve(path, cmd->full_cmd, envp);
			perror(path);
			free(path);
			free_str_array(envp);
			exit(127);
		}
		else if (pid < 0)
		{
			perror("fork");
			free_str_array(envp);
			if (prev_fd != -1)
				close(prev_fd);
			if (cmd->next)
			{
				close(pipefd[0]);
				close(pipefd[1]);
			}
			return (1);
		}
		// Parent process
		if (prev_fd != -1)
			close(prev_fd);
		if (cmd->next)
		{
			close(pipefd[1]);
			prev_fd = pipefd[0];
		}
		cmd = cmd->next;
	}
	// FIXED: Close final pipe fd if exists
	if (prev_fd != -1)
		close(prev_fd);
	// Wait for all children
	while (wait(NULL) > 0)
		;
	free_str_array(envp);
	return (0);
}

// ============================ SIGNALS ============================ //

void	handle_sigint(int sig)
{
	(void)sig;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	handle_signals(void)
{
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
}
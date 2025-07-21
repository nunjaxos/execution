#include "minishell.h"

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

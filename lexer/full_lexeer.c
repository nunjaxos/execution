#include "minishell.h"

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

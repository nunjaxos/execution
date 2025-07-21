/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 18:41:02 by abnemili          #+#    #+#             */
/*   Updated: 2025/06/26 22:41:53 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
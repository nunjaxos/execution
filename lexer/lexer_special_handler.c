/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_special_handler.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 15:07:59 by abnemili          #+#    #+#             */
/*   Updated: 2025/06/27 13:42:53 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
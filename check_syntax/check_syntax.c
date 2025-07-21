/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_syntax.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 14:17:58 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/12 20:14:05 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

int	is_quote(enum e_type type)
{
	return (type == QUOTE || type == DQUOTE);
}

char	*get_dredir_error(t_elem *curr)
{
	t_elem	*next;

	next = skip_whitespace(curr->next);
	if (!next || !is_redirection(next->type))
	{
		return ("newline");
	}
	if (next->type == curr->type)
	{
		if (curr->type == DREDIR_OUT)
			return (">>");
		else
			return ("<<<");
	}
	if (next->type == REDIR_OUT)
	{
		return (">");
	}
	if (next->type == REDIR_IN)
	{
		return ("<");
	}
	return ("newline");
}

char	*get_sredir_error(t_elem *curr)
{
	t_elem	*next;
	t_elem	*third;

	next = skip_whitespace(curr->next);
	if (!next || next->type != curr->type)
	{
		return ("newline");
	}
	third = skip_whitespace(next->next);
	if (third && third->type == curr->type)
	{
		if (curr->type == REDIR_OUT)
		{
			return (">");// Map enum e_type 0to string
			
		}
		else
		{
			return ("<");
		}
	}
	return ("newline");
}
const char	*get_type_str(enum e_type type)
{
	switch (type)
	{
	case WORD:
		return ("WORD");
	case WHITE_SPACE:
		return ("WHITE_SPACE");
	case REDIR_IN:
		return ("REDIR_IN");
	case REDIR_OUT:
		return ("REDIR_OUT");
	case DREDIR_OUT:
		return ("DREDIR_OUT");
	case HERE_DOC:
		return ("HERE_DOC");
	case PIPE_LINE:
		return ("PIPE_LINE");
	case QUOTE:
		return ("QUOTE");
	case ENV:
		return ("ENV");
	case DQUOTE:
		return ("DQUOTE");
	case EXIT_STATUS:
		return ("EXIT_STATUS");
	default:
		return ("(unknown)");
	}
}

// Map enum e_state to string safely
const char	*get_state_str(enum e_state state)
{
	switch (state)
	{
	case GENERAL:
		return ("GENERAL");
	case IN_QUOTE:
		return ("IN_QUOTE");
	case IN_DQUOTE:
		return ("IN_DQUOTE");
	default:
		return ("UNKNOWN_STATE");
	}
}

// Print all tokens in the list
void	print_tokens(t_elem *head)
{
	while (head)
	{
		printf("Content: %-15s | Type: %-12s | State: %-10s\n", head->content,
			get_type_str(head->type), get_state_str(head->state));
		head = head->next;
	}
}

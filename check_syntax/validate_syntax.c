/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate_syntax.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 10:25:16 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/01 14:29:17 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	validate_pipe(t_elem *prev_significant)
{
	if (!prev_significant || prev_significant->type == PIPE_LINE
		|| is_redirection(prev_significant->type))
	{
		printf("minishell: syntax error near unexpected token `|'\n");
		return (0);
	}
	return (1);
}

t_elem	*skip_whitespace(t_elem *token)
{
	while (token && token->type == WHITE_SPACE)
		token = token->next;
	return (token);
}

int	validate_redirection_target(t_elem *next)
{
	if (!next)
	{
		printf("minishell: syntax error: unexpected end of file\n");
		return (0);
	}
	if (next->type == PIPE_LINE)
	{
		printf("minishell: syntax error near unexpected token `|'\n");
		return (0);
	}
	if (is_redirection(next->type))
	{
		printf("minishell: syntax error near unexpected token `%s'\n",
			get_redirection_symbol(next->type));
		return (0);
	}
	if (next->type != WORD && !is_quote(next->type))
	{
		printf("minishell: syntax error near unexpected token\n");
		return (0);
	}
	return (1);
}

int	validate_redirection(t_elem *curr)
{
	t_elem	*next;

	next = skip_whitespace(curr->next);
	return (validate_redirection_target(next));
}

int	check_final_syntax(enum e_state state, t_elem *prev_significant)
{
	if (state != GENERAL)
	{
		if (state == IN_QUOTE)
			printf("minishell: unexpected EOF while looking for matching `''\n");
		else
			printf("minishell: unexpected EOF while looking for matching `\"'\n");
		return (0);
	}
	if (prev_significant && (prev_significant->type == PIPE_LINE
			|| is_redirection(prev_significant->type)))
	{
		if (prev_significant->type == PIPE_LINE)
		{
			printf("minishell: syntax error near unexpected token `|'\n");
		}
		else
			printf("minishell: syntax error: unexpected end of file\n");
		return (0);
	}
	return (1);
}

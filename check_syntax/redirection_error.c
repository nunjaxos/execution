/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_error.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/28 23:35:03 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/01 14:18:01 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	check_initial_syntax(t_elem *curr)
{
	curr = skip_whitespace(curr);
	if (!curr || (curr->type != PIPE_LINE && !is_redirection(curr->type)))
		return (1);
	if (curr->type == PIPE_LINE)
		ft_putstr_fd("minishell: syntax error near unexpectes token '|'\n", 1);
	else
		printf("minishell: syntax error near unexpectes token '%s'\n",
			get_error_token(curr));
	return (0);
}

char	*get_error_token(t_elem *curr)
{
	if (curr->type == REDIR_IN || curr->type == REDIR_OUT)
		return (get_sredir_error(curr));
	if (curr->type == HERE_DOC || curr->type == DREDIR_OUT)
		return (get_dredir_error(curr));
	return ("newline");
}

static void	ft_handle_quote(enum e_state *state)
{
	if (*state == IN_QUOTE)
		*state = GENERAL;
	else
		*state = IN_QUOTE;
}

static void	ft_handle_dquote(enum e_state *state)
{
	if (*state == IN_DQUOTE)
		*state = GENERAL;
	else
		*state = IN_DQUOTE;
}

int	check_unclosed_quotes_in_tokens(t_elem *tokens)
{
	t_elem			*curr;
	enum e_state	state;

	curr = tokens;
	state = GENERAL;
	while (curr)
	{
		if (curr->type == QUOTE)
			if (state != IN_DQUOTE)
				ft_handle_quote(&state);
		if (curr->type == DQUOTE)
			if (state != IN_QUOTE)
				ft_handle_dquote(&state);
		curr = curr->next;
	}
	if (state == GENERAL)
		return (1);
	printf("minishell: syntax error: unclosed quote\n");
	return (0);
}

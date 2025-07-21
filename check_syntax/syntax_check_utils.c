/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_check_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 14:17:58 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/01 14:25:30 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

static void	ft_handle_single_quote(char *in_quote)
{
	if (*in_quote == '\'')
		*in_quote = 0;
	else
		*in_quote = '\'';
}

static void	ft_handle_double_quote(char *in_quote)
{
	if (*in_quote == '"')
		*in_quote = 0;
	else
		*in_quote = '"';
}

int	check_unclosed_quotes_in_input(const char *input)
{
	int		i;
	char	in_quote;

	i = 0;
	in_quote = 0;
	while (input[i])
	{
		if (input[i] == '\'')
			if (in_quote != '"')
				ft_handle_single_quote(&in_quote);
		if (input[i] == '"')
			if (in_quote != '\'')
				ft_handle_double_quote(&in_quote);
		i++;
	}
	if (in_quote != 0)
	{
		printf("minishell: syntax error: unclosed quote\n");
		return (0);
	}
	return (1);
}

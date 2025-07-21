/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 14:24:56 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/01 14:25:34 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

int	update_quote_state(enum e_type type, enum e_state *state)
{
	if (type == QUOTE && *state != IN_DQUOTE)
	{
		if (*state == IN_QUOTE)
			*state = GENERAL;
		else
			*state = IN_QUOTE;
	}
	else if (type == DQUOTE && *state != IN_QUOTE)
	{
		if (*state == IN_DQUOTE)
			*state = GENERAL;
		else
			*state = IN_DQUOTE;
	}
	return (1);
}

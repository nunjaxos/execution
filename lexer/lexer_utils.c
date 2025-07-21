/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 22:45:07 by abnemili          #+#    #+#             */
/*   Updated: 2025/06/27 14:32:33 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
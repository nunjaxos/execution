/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word_handler.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 22:13:06 by abnemili          #+#    #+#             */
/*   Updated: 2025/06/27 14:23:49 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

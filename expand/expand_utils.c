/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/29 11:41:15 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/21 09:52:57 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"



char	*remove_quotes(char *content, enum e_type quote_type)
{
	int		len;
	char	quote_char;

	if (!content)
		return (NULL);
	len = strlen(content);
	if (len < 2)
		return (ft_strdup(content));
	if (quote_type == QUOTE)
		quote_char = '\'';
	else
		quote_char = '"';
	if (content[0] == quote_char && content[len - 1] == quote_char)
		return (ft_strndup(content + 1, len - 2));
	return (ft_strdup(content));
}

void	handle_quoted_token(t_elem *curr, int exit_code)
{
	char	*unq;
	char	*exp;
	int		sh_expand;

	unq = remove_quotes(curr->content, curr->type);
	if (!unq)
		return ;
	sh_expand = (curr->type == DQUOTE);
	exp = expand_token_content(unq, exit_code, sh_expand);
	free(unq);
	if (exp)
	{
		free(curr->content);
		curr->content = exp;
		curr->type = WORD;
	}
}

int	process_dollar_expansion(char *content, int *i, t_expand_data *data)
{
	int		var_end;
	char	*name;
	char	*value;
	int		is_special;

	name = extract_var_name(content, *i, &var_end);
	if (name)
	{
		is_special = handle_special_var(name, data->exit_code, &value);
		if (!copy_var_value(data->res, data->len, data->max, value))
		{
			cleanup_var_expansion(name, value, is_special);
			return (0);
		}
		cleanup_var_expansion(name, value, is_special);
		*i = var_end;
		return (1);
	}
	if (!(*(data->res) = realloc_result(*(data->res), data->max, *(data->len)
				+ 2)))
		return (0);
	(*(data->res))[(*(data->len))++] = '$';
	return (1);
}

char	*realloc_result(char *result, int *max_size, int needed)
{
	char	*new_result;

	if (needed < *max_size)
		return (result);
	*max_size = needed + 100;
	new_result = realloc(result, *max_size);
	if (!new_result)
	{
		free(result);
		return (NULL);
	}
	return (new_result);
}
int	handle_special_var(char *name, int exit_code, char **value)
{
	if (strcmp(name, "?") == 0)
	{
		*value = expand_exit_status(exit_code);
		return (1);
	}
	*value = get_env_value(name);
	return (0);
}
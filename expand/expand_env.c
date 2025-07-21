/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 14:26:24 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/21 09:53:49 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


// char	*get_env_value(char *name)
// {
// 	t_env	*temp;

// 	temp = g_envp;
// 	if (!name)
// 		return (NULL);
// 	while (temp)
// 	{
// 		if (strcmp(temp->name, name) == 0)
// 			return (temp->value);
// 		temp = temp->next;
// 	}
// 	return (NULL);
// }

int	is_valid_var_char(char c)
{
	return (ft_isalnum(c) || c == '_');
}

int	copy_var_value(char **res, int *len, int *max, char *val)
{
	int		vlen;
	char	*temp;

	if (!val)
		val = "";
	vlen = strlen(val);
	temp = realloc_result(*res, max, *len + vlen + 1);
	if (!temp)
		return (0);
	*res = temp;
	strcpy(*res + *len, val);
	*len += vlen;
	return (1);
}

char	*extract_var_name(char *str, int start, int *end)
{
	int	i;

	i = start;
	if (str[i] == '?')
	{
		*end = i + 1;
		return (ft_strdup("?"));
	}
	while (str[i] && is_valid_var_char(str[i]))
		i++;
	*end = i;
	if (i == start)
		return (NULL);
	return (ft_strndup(str + start, i - start));
}

char	*expand_exit_status(int exit_code)
{
	return (ft_itoa(exit_code));
}

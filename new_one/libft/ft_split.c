/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 21:42:49 by amouhand          #+#    #+#             */
/*   Updated: 2023/11/18 03:11:44 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	sub_free(char **strs, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n)
	{
		free(strs[i]);
		i++;
	}
	free(strs);
}

static char	*sub_copy(char const *string, char *sub, char delimiter, size_t *j)
{
	size_t	i;

	i = 0;
	while (string[*j] && string[*j] == delimiter)
		(*j)++;
	while (string[*j] && string[*j] != delimiter)
	{
		sub[i] = string[*j];
		(*j)++;
		i++;
	}
	sub[i] = '\0';
	return (sub);
}

static char	*sub_allocation(char const *string, char delimiter, \
	size_t *i, size_t *j)
{
	size_t		len;
	char		*sub;

	len = 0;
	while (string[*i])
	{
		while (string[*i] == delimiter && string[*i])
			(*i)++;
		while (string[*i] && string[*i] != delimiter)
		{
			len++;
			(*i)++;
		}
		break ;
	}
	sub = malloc(sizeof(char) * (len + 1));
	if (!sub)
		return (NULL);
	sub = sub_copy(string, sub, delimiter, j);
	return (sub);
}

static size_t	wordcount(char const *string, char delimiter)
{
	size_t	i;
	size_t	count;
	size_t	check;

	i = 0;
	count = 0;
	if (!string)
		return (0);
	while (string[i])
	{
		check = 0;
		while (string[i] == delimiter && string[i])
			i++;
		while (string[i] && string[i] != delimiter)
		{
			check = 1;
			i++;
		}
		if (check == 1)
			count++;
	}
	return (count);
}

char	**ft_split(char const *s, char c)
{
	size_t		count;
	char		**strs;
	size_t		n;
	size_t		i;
	size_t		j;

	j = 0;
	i = 0;
	n = 0;
	count = wordcount(s, c);
	strs = (char **)malloc(sizeof(char *) * (count + 1));
	if (!strs)
		return (NULL);
	strs[count] = NULL;
	while (n < count)
	{
		strs[n] = sub_allocation(s, c, &i, &j);
		if (!strs[n])
		{
			sub_free(strs, n);
			return (NULL);
		}
		n++;
	}
	return (strs);
}

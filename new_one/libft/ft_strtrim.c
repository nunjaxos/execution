/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 16:15:43 by amouhand          #+#    #+#             */
/*   Updated: 2023/11/18 02:58:33 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	is_banned(char c, const char *set)
{
	int	i;

	i = 0;
	while (set[i])
	{
		if (set[i] == c)
			return (1);
		i++;
	}
	return (0);
}

static size_t	skip(const char *src, char const *set)
{
	size_t	i;

	i = 0;
	while (src[i])
	{
		if (!is_banned(src[i], set))
			break ;
		i++;
	}
	return (i);
}

static int	end_skip(const char *src, const char *set, size_t len)
{
	size_t	i;

	i = len - 1;
	while (src[i])
	{
		if (!is_banned(src[i], set))
			break ;
		i--;
	}
	return (i);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	char	*cut;
	size_t	index;
	size_t	end;
	size_t	start;

	if (!s1 || !set)
		return (NULL);
	index = 0;
	start = skip(s1, set);
	end = end_skip(s1, set, ft_strlen(s1));
	if (start == ft_strlen(s1))
		return (ft_strdup(""));
	cut = (char *)malloc(sizeof(char) * ((end - start) + 2));
	if (!cut)
		return (NULL);
	if (!ft_strlen(s1))
		return (NULL);
	while (start <= end)
	{
		cut[index++] = s1[start++];
	}
	cut[index] = '\0';
	return (cut);
}

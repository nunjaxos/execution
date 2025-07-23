/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <amouhand@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/22 20:37:49 by amouhand          #+#    #+#             */
/*   Updated: 2024/08/22 01:18:28 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlen_gnl(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
	{
		i++;
	}
	return (i);
}

static char	*ft_strdup_gnl(char *s)
{
	size_t	i;
	size_t	len;
	char	*string;

	len = ft_strlen_gnl(s);
	i = 0;
	string = (char *)malloc(sizeof(char) * (len + 1));
	if (!string)
		return (NULL);
	while (s[i])
	{
		string[i] = s[i];
		i++;
	}
	string[i] = '\0';
	return (string);
}

static char	*ft_joined(char *joined, const char *s1, const char *s2)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	while (s1[i] || s2[j])
	{
		if (s1[i])
		{
			joined[i] = s1[i];
			i++;
		}
		else if (s2[j])
		{
			joined[i + j] = s2[j];
			j++;
		}
	}
	joined[i + j] = '\0';
	return (joined);
}

char	*ft_strjoin_gnl(char *s1, char *s2)
{
	size_t	total_len;
	char	*joined;

	if (!s1 && !s2)
		return (NULL);
	else if (s1 && !s2)
		return (ft_strdup_gnl(s1));
	else if (!s1 && s2)
		return (ft_strdup_gnl(s2));
	total_len = ft_strlen_gnl(s1) + ft_strlen_gnl(s2);
	joined = (char *)malloc(total_len * sizeof(char) + 1);
	if (!joined)
		return (NULL);
	joined = ft_joined(joined, s1, s2);
	free(s1);
	return (joined);
}

char	*ft_strchr_gnl(const char *s, int c)
{
	unsigned int	i;

	i = 0;
	if (!s)
		return (NULL);
	if (c == '\0')
		return ((char *)&s[ft_strlen_gnl(s)]);
	while (s[i])
	{
		if (s[i] == (char)c)
			return ((char *)&s[i]);
		i++;
	}
	return (0);
}

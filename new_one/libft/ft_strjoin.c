/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 14:23:41 by amouhand          #+#    #+#             */
/*   Updated: 2023/11/19 02:22:12 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

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
	return (0);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t		total_len;
	char		*joined;

	if (!s1 && s2)
		return (ft_strdup(s2));
	else if (!s2 && s1)
		return (ft_strdup(s1));
	else if (!s1 && !s2)
		return (ft_strdup(""));
	total_len = ft_strlen(s1) + ft_strlen(s2);
	joined = (char *)ft_calloc((total_len + 1), sizeof(char));
	if (!joined)
		return (NULL);
	ft_joined(joined, s1, s2);
	return (joined);
}

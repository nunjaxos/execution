/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 20:51:06 by amouhand          #+#    #+#             */
/*   Updated: 2023/11/19 02:16:38 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *s)
{
	size_t	i;
	size_t	len;
	char	*string;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	i = 0;
	string = (char *)malloc(sizeof(char) *(len + 1));
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

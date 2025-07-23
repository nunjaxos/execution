/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 23:36:25 by amouhand          #+#    #+#             */
/*   Updated: 2023/11/18 22:25:00 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*sub;
	size_t	src_len;
	size_t	length;

	length = 0;
	if (!s)
		return (NULL);
	src_len = ft_strlen(s);
	if (start >= src_len)
		return (ft_strdup(""));
	if (start < src_len)
		length = src_len - start;
	if (length > len)
		length = len;
	sub = malloc(sizeof(char) * (length + 1));
	if (!sub)
		return (NULL);
	ft_strlcpy(sub, s + start, length + 1);
	return (sub);
}

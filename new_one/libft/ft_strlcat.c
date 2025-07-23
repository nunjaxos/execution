/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 22:55:41 by amouhand          #+#    #+#             */
/*   Updated: 2023/11/18 23:39:46 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	size_t	i;
	size_t	j;
	size_t	len_src;
	size_t	len_dest;

	if (!dstsize)
		return (ft_strlen(src));
	len_src = ft_strlen(src);
	len_dest = ft_strlen(dst);
	i = ft_strlen(dst);
	j = 0;
	if (dstsize <= len_dest)
		return (dstsize + len_src);
	while (src[j] != '\0' && i + 1 < dstsize)
	{
		dst[i] = src[j];
		j++;
		i++;
	}
	dst[i] = '\0';
	return (len_dest + len_src);
}

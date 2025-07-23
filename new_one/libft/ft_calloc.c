/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/05 18:18:38 by amouhand          #+#    #+#             */
/*   Updated: 2023/11/19 10:56:02 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*calloced;

	if (size && nmemb > SIZE_MAX / size)
		return (NULL);
	calloced = malloc((size) * (nmemb));
	if (!calloced)
		return (NULL);
	if ((nmemb * size) == 0)
		ft_bzero(calloced, 1);
	else
		ft_memset(calloced, 0, (size * nmemb));
	return (calloced);
}

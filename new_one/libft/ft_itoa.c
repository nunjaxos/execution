/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/04 20:02:20 by amouhand          #+#    #+#             */
/*   Updated: 2023/11/19 02:12:20 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>

static unsigned int	number_count(int n)
{
	unsigned int	length;

	length = 0;
	if (n == 0)
		length = 1;
	if (n < 0)
	{
		length++;
	}
	while (n != 0)
	{
		n /= 10;
		length++;
	}
	return (length);
}

char	*ft_itoa(int n)
{
	char			*result;
	unsigned int	count;
	unsigned int	number;

	count = number_count(n);
	number = n;
	result = (char *)malloc((sizeof(char)) * (count + 1));
	if (!result)
		return (NULL);
	if (number == 0)
		result[0] = '0';
	if (n < 0)
	{
		result[0] = '-';
		number = -n;
	}
	result[count] = '\0';
	while (number > 0)
	{
		result[count - 1] = (number % 10) + '0';
		number = number / 10;
		count--;
	}
	return (result);
}

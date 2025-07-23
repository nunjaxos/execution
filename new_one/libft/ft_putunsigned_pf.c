/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putunsigned_pf.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 22:24:45 by amouhand          #+#    #+#             */
/*   Updated: 2024/03/18 05:39:35 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_putunsigned_pf(unsigned int n, int *len)
{
	if (n >= 10)
	{
		ft_putunsigned_pf(n / 10, len);
		ft_putunsigned_pf(n % 10, len);
	}
	else
	{
		(*len) += ft_putchar_pf((n + '0'));
	}
	return (*len);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_pf.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 23:41:14 by amouhand          #+#    #+#             */
/*   Updated: 2024/03/18 05:43:45 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_putnbr_pf(int n, int *len)
{
	if (n == INT_MIN)
		return (ft_putstr_pf("-2147483648"));
	if (n < 0)
	{
		(*len) += ft_putchar_pf('-');
		ft_putnbr_pf(-n, len);
	}
	else if (n >= 10)
	{
		ft_putnbr_pf(n / 10, len);
		ft_putnbr_pf(n % 10, len);
	}
	else
	{
		(*len) += ft_putchar_pf((n + '0'));
	}
	return (*len);
}

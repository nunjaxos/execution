/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/24 00:01:58 by amouhand          #+#    #+#             */
/*   Updated: 2024/03/18 06:19:46 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	check(char c, va_list lst)
{
	int	len;
	int	flag;
	int	nill_check;

	len = 0;
	flag = 0;
	nill_check = 0;
	if (c == 'c')
		len = ft_putchar_pf(va_arg(lst, int));
	else if (c == 's')
		len = ft_putstr_pf(va_arg(lst, char *));
	else if (c == 'd' || c == 'i')
		len = ft_putnbr_pf(va_arg(lst, int), &len);
	else if (c == '%')
		len = ft_putchar_pf('%');
	else if (c == 'x' || c == 'X')
		len = ft_printhexa_pf(va_arg(lst, unsigned int), c, &len, &flag);
	else if (c == 'p')
		len = ft_ptr_pf(va_arg(lst, unsigned long), &flag, &len, &nill_check);
	else if (c == 'u')
		len = ft_putunsigned_pf(va_arg(lst, unsigned int), &len);
	return (len);
}

int	ft_printf(const char *s, ...)
{
	va_list	lst;
	size_t	i;
	int		len;

	i = 0;
	len = 0;
	if (!s || (s[i] == '%' && !s[i + 1]))
		return (-1);
	va_start(lst, s);
	while (s[i])
	{
		if (s[i] == '%' && !s[i + 1])
			return (-1);
		else if (s[i] == '%')
		{
			len += check(s[i + 1], lst);
			i++;
		}
		else
			len += ft_putchar_pf(s[i]);
		i++;
	}
	return (len);
}

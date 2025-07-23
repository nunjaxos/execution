/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstsize_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 00:27:08 by amouhand          #+#    #+#             */
/*   Updated: 2023/11/17 01:05:06 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_lstsize(t_list *lst)
{
	int		count;
	t_list	*nodes;

	if (!lst)
		return (0);
	count = 0;
	nodes = lst;
	while (nodes)
	{
		count++;
		nodes = nodes->next;
	}
	return (count);
}

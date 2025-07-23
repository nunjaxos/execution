/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 20:51:29 by amouhand          #+#    #+#             */
/*   Updated: 2023/11/11 00:16:18 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void*))
{
	t_list	*ptr;
	t_list	*store;

	if (!lst || !del)
		return ;
	ptr = *lst;
	store = NULL;
	while (ptr)
	{
		store = ptr->next;
		ft_lstdelone(ptr, del);
		ptr = store;
	}
	*lst = NULL;
}

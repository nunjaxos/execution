/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 23:55:21 by amouhand          #+#    #+#             */
/*   Updated: 2023/11/18 02:06:28 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*ptr;
	t_list	*new;
	void	*c;

	if (!del || !f || !lst)
		return (0);
	new = NULL;
	while (lst)
	{
		c = f(lst->content);
		ptr = ft_lstnew(c);
		ft_lstadd_back(&new, ptr);
		if (!ptr)
		{
			ft_lstclear(&new, del);
			del(c);
			return (0);
		}
		lst = lst->next;
	}
	return (new);
}

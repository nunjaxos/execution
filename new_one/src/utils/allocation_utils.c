#include "../../include/executor.h"

void	*ft_malloc(size_t size)
{
	void	*allocated;

	allocated = malloc(size);
	if (!allocated)
		return (NULL);
	add_alloc(allocated);
	return (allocated);
}

void	add_alloc(void *allocated)
{
	t_alloc	*list;
	t_alloc	*new_alloc;

	new_alloc = malloc(sizeof(t_alloc));
	if (!new_alloc)
		return ;
	list = get_data()->alloc;
	new_alloc->allocated = allocated;
	new_alloc->next = NULL;
	if (!list)
		get_data()->alloc = new_alloc;
	else
	{
		while (list->next)
			list = list->next;
		list->next = new_alloc;
	}
}


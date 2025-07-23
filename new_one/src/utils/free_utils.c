#include "../../include/executor.h"

void	garbage_removal(void)
{
	t_alloc	*list;
	t_alloc	*tmp;

	list = get_data()->alloc;
	while (list)
	{
		tmp = list->next;
		if (list->allocated)
		{
			free(list->allocated);
			list->allocated = NULL;
		}
		if (list)
		{
			free(list);
			list = NULL;
		}
		list = tmp;
	}
	get_data()->alloc = NULL;
}

static void	remove_from_list(t_alloc **list, t_alloc *prev)
{
	if (prev)
		prev->next = (*list)->next;
	else
		get_data()->alloc = (*list)->next;
}

static void	free_allocated_memory(t_alloc *list)
{
	if (list->allocated)
	{
		free(list->allocated);
		list->allocated = NULL;
	}
}

static void	free_list_node(t_alloc *list)
{
	if (list)
	{
		free(list);
		list = NULL;
	}
}

void	ft_free(void *ptr)
{
	t_alloc	*list;
	t_alloc	*prev;

	if (!ptr)
		return ;
	list = get_data()->alloc;
	prev = NULL;
	while (list)
	{
		if (list->allocated == ptr)
		{
			remove_from_list(&list, prev);
			free_allocated_memory(list);
			free_list_node(list);
			break ;
		}
		prev = list;
		list = list->next;
	}
}

void	free_data(t_data *data)
{
	garbage_removal();
	free(data);
	data = NULL;
	rl_clear_history();
}
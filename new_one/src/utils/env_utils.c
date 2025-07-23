#include "../../include/executor.h"

t_env	*copy_env_list(t_env *env)
{
	t_env	*tmp;
	t_env	*new;
	t_env	*head;

	tmp = env;
	head = NULL;
	while (tmp)
	{
		new = ft_malloc(sizeof(t_env));
		if (!new)
			return (NULL);
		new->name = ft_strdup(tmp->name);
		new->value = ft_strdup(tmp->value);
		new->next = NULL;
		if (!head)
			head = new;
		else
		{
			new->next = head;
			head = new;
		}
		tmp = tmp->next;
	}
	return (head);
}

void	sort_env_list(t_env *env)
{
	t_env	*tmp;
	t_env	*tmp2;
	char	*temp;

	tmp = env;
	while (tmp)
	{
		tmp2 = tmp->next;
		while (tmp2)
		{
			if (ft_strcmp(tmp->name, tmp2->name) > 0)
			{
				temp = tmp->name;
				tmp->name = tmp2->name;
				tmp2->name = temp;
				temp = tmp->value;
				tmp->value = tmp2->value;
				tmp2->value = temp;
			}
			tmp2 = tmp2->next;
		}
		tmp = tmp->next;
	}
}

void	free_env(t_env *head)
{
	t_env	*current;
	t_env	*next;

	current = head;
	while (current)
	{
		next = current->next;
		if (current->name)
		{
			ft_free(current->name);
			current->name = NULL;
		}
		if (current->value)
		{
			ft_free(current->value);
			current->value = NULL;
		}
		if (current)
		{
			ft_free(current);
			current = NULL;
		}
		current = next;
	}
	current = NULL;
}

int	env_name_exists(t_env *env, char *name)
{
	t_env	*tmp;

	tmp = env;
	while (tmp)
	{
		if (!ft_strcmp(tmp->name, name))
			return (1);
		tmp = tmp->next;
	}
	return (0);
}

void	edit_env(t_env *head, char *name ,char *new_value)
{
	t_env	*current;

	current = head;
	while (current)
	{
		if (!ft_strcmp(current->name ,name))
		{
			current->value = new_value;
			return ;
		}
		current = current->next;
	}
}

void	add_env(t_env **head, char *name, char *value)
{
	t_env	*new;
	t_env	*last;

	new = malloc(sizeof(t_env));
	if (!new)
	{
		free_env(*head);
		return ;
	}
	new->name = name;
	new->value = value;
	new->next = NULL;
	if (!*head)
		*head = new;
	else
	{
		last = *head;
		while (last->next)
			last = last->next;
		last->next = new;
	}
}

void	env_update(t_env *env)
{
	if (!get_value("OLDPWD", env))
		add_env(&env, "OLDPWD", NULL);
	check_shlvl(env);
}

t_env *create_env_node(const char *env)
{
	t_env *node;

	node = ft_malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	char *sep = strchr(env, '=');
	if (!sep)
		return (NULL); // Don't free node — it’s tracked
	node->name = ft_strdup_track(env, sep - env);
	if (!node->name)
		return (NULL);
	node->value = ft_strdup_track(sep + 1, -1); // -1 = till null
	if (!node->value)
		return (NULL);
	node->next = NULL;
	return node;
}


void add_env_back(t_env **lst, t_env *new_node)
{
	if (!lst || !new_node)
		return;
	if (!*lst)
	{
		*lst = new_node;
		return;
	}
	t_env *temp = *lst;
	while (temp->next)
		temp = temp->next;
	temp->next = new_node;
}

void	init_env_list(char **envp, t_data *data)
{
	int		i = 0;
	t_env	*node;

	while (envp[i])
	{
		node = create_env_node(envp[i]);
		if (node)
			add_env_back(&data->n_env, node);
		i++;
	}
}

void free_env_list(t_env *env)
{
	t_env *tmp;
	while (env)
	{
		tmp = env;
		env = env->next;
		free(tmp->name);
		free(tmp->value);
		free(tmp);
	}
}
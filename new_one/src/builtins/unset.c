#include "../../include/executor.h"

void	delete_env(t_env *env, char *name)
{
	t_env	*tmp;
	t_env	*prev;

	tmp = env;
	prev = NULL;
	while (tmp)
	{
		if (!ft_strcmp(tmp->name, name))
		{
			if (prev)
				prev->next = tmp->next;
			else
				env = tmp->next;
			ft_free(tmp->name);
			if (tmp->value)
				ft_free(tmp->value);
			ft_free(tmp);
			return ;
		}
		prev = tmp;
		tmp = tmp->next;
	}
}

void	ft_unset(t_cmd *cmd, pid_t pid)
{
	int	i;

	i = 1;
	while (cmd->full_cmd[i])
	{
		if (env_name_exists(get_data()->n_env, cmd->full_cmd[i]))
			delete_env(get_data()->n_env, cmd->full_cmd[i]);
		i++;
	}
	check_for_child(pid, 0);
}

// int	main(int argc, char **argv, char **envp)
// {
// 	(void)argc;
// 	(void)argv;

// 	t_data	*data;
// 	t_cmd	cmd;

// 	data = malloc(sizeof(t_data));            // ✅ allocate memory
// 	if (!data)
// 		return (1);                          // handle allocation failure
// 	ft_bzero(data, sizeof(t_data));          // ✅ zero out actual structure

// 	// Now safe to use
// 	init_env_list(envp, data);

// 	printf("=== ENV BEFORE UNSET ===\n");
// 	print_export_list(data->n_env);
// 	printf("yppppp\n");
// 	char *unset1[] = {"unset", "PATH", NULL};
// 	cmd.full_cmd = unset1;
// 	cmd.pid = 0;
// 	ft_unset(&cmd, cmd.pid);
// 	printf("nooo\n");
// 	char *unset2[] = {"unset", "HOME", NULL};
// 	cmd.full_cmd = unset2;
// 	ft_unset(&cmd, cmd.pid);

// 	char *unset3[] = {"unset", "NOT_PRESENT", NULL};
// 	cmd.full_cmd = unset3;
// 	ft_unset(&cmd, cmd.pid);

// 	printf("\n=== ENV AFTER UNSET ===\n");
// 	print_export_list(data->n_env);

// 	free_env(data->n_env);
// 	garbage_removal();
// 	free(data);  // ✅ don't forget to free the struct itself

// 	return 0;
// }

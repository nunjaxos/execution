#include "../../include/executor.h"

int	export_error_display(t_cmd *cmd, char *name, char *value, int *i)
{
	char	*tmp;

	tmp = NULL;
	if (!check_valid_name(name))
	{
		export_not_valid(tmp, i, value, name);
		return (1);
	}
	if (str_index(cmd->full_cmd[*i], '=')
		&& cmd->full_cmd[*i][str_index(cmd->full_cmd[*i], '=') - 1] == '+'
		&& env_name_exists(get_data()->n_env, name))
		edit_env(get_data()->n_env, name,
			ft_strjoin_free(get_value(name, get_data()->n_env), value, 1));
	else if (str_index(cmd->full_cmd[*i], '=')
		&& cmd->full_cmd[*i][str_index(cmd->full_cmd[*i], '=') - 1] == '+')
		add_env(&get_data()->n_env, ft_strdup(name), ft_strdup(value));
	else if (env_name_exists(get_data()->n_env, name))
		edit_env(get_data()->n_env, name, ft_strdup(value));
	else
		add_env(&get_data()->n_env, ft_strdup(name), ft_strdup(value));
	return (0);
}

int	export_null(int *i, t_cmd *cmd)
{
	char	*name;

	name = ft_strdup(cmd->full_cmd[*i]);
	if (!check_valid_name(name))
	{
		ft_putstr_fd("minishell: export: `", 2);
		if (ft_strlen(name))
			ft_putstr_fd(name, 2);
		else
			ft_putstr_fd("=", 2);
		ft_putstr_fd("': not a valid identifier\n", 2);
		*i += 1;
		ft_free(name);
		get_data()->exit_status = 1;
		return (1);
	}
	if (env_name_exists(get_data()->n_env, name))
	{
		if (!get_value(name, get_data()->n_env))
			edit_env(get_data()->n_env, ft_strdup(name), NULL);
	}
	else
		add_env(&get_data()->n_env, ft_strdup(name), NULL);
	return (0);
}

void	export_args_handle(t_cmd *cmd, char *key, char *value)
{
	int	i;

	i = 1;
	while (cmd->full_cmd[i])
	{
		if (ft_strchr(cmd->full_cmd[i], '='))
		{
			value = ft_strchr(cmd->full_cmd[i], '=') + 1;
			key = ft_substr(cmd->full_cmd[i], 0, key_len(cmd->full_cmd[i]));
			if (export_error_display(cmd, key, value, &i))
				continue ;
		}
		else if (export_null(&i, cmd))
			continue ;
		i++;
		ft_free(key);
	}
	check_for_child(cmd->pid, get_data()->exit_status);
}

int	ft_export(t_cmd *cmd)
{
	char	*key;
	char	*value;

	key = NULL;
	value = NULL;
	if (cmd->count == 1)
		print_export_list(get_data()->n_env);
	else
		export_args_handle(cmd, key, value);
	return (1);
}

void	print_export_list(t_env *env)
{
	t_env	*tmp;
	t_env	*to_free;

	tmp = copy_env_list(env);
	if (!tmp)
		return ;
	sort_env_list(tmp);
	to_free = tmp;
	while (tmp)
	{
		ft_putstr_fd("declare -x ", 1);
		ft_putstr_fd(tmp->name, 1);
		if (tmp->value)
		{
			ft_putstr_fd("=\"", 1);
			ft_putstr_fd(tmp->value, 1);
			ft_putchar_fd('\"', 1);
		}
		ft_putchar_fd('\n', 1);
		tmp = tmp->next;
	}
	free_env(to_free);
}

// int	main(int argc, char **argv, char **envp)
// {
// 	(void)argc;
// 	(void)argv;

// 	t_data	data;
// 	t_cmd	cmd;

// 	// Sample input like: export NAME="Code" NEWVAR=42 NEWVAR+=plus INVALID+VAR=x
// 	char *args[] = {"export", "NAME=Code", "NEWVAR=42", "NEWVAR+=plus", NULL};

// 	ft_bzero(&data, sizeof(t_data));
// 	init_env_list(envp, &data);

// 	cmd.full_cmd = args;
// 	cmd.pid = 0;

// 	printf("=== BEFORE EXPORT ===\n");
// 	print_export_list(data.n_env);

// 	ft_export(&cmd);
// 	printf("\n=== AFTER EXPORT ===\n");
// 	print_export_list(data.n_env);

// 	// Clean-up
// 	garbage_removal();

// 	return (0);
// }
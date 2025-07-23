#include "../../include/executor.h"

char	*get_valid_path(t_cmd *cmd, pid_t pid)
{
	char	*path;

	path = cd_error(cmd, pid);
	if (!ft_strcmp(cmd->full_cmd[1], "~") || cmd->count == 1)
	{
		path = get_value("HOME", get_data()->n_env);
		if (!path)
		{
			ft_putstr_fd("cd: Error: HOME not set\n", 2);
			get_data()->exit_status = 1;
			check_for_child(pid, 1);
			return (NULL);
		}
	}
	return (path);
}

void	update_pwd(t_env *env)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	add_alloc(pwd);
	if (get_value("PWD", env))
	{
		ft_free(get_value("PWD", env));
		edit_env(env, "PWD", ft_strdup(pwd));
	}
	ft_free(pwd);
}

void	check_shlvl(t_env *env)
{
	int	shlvl;

	shlvl = ft_atoi_shlvl(get_value("SHLVL", env));
	edit_env(env, "SHLVL", ft_itoa(shlvl + 1));
}

int	ft_atoi_shlvl(char *shlvl)
{
	int	i;
	int	nbr;

	i = 0;
	nbr = 0;
	while (shlvl[i])
	{
		if (!ft_isdigit(shlvl[i]))
			return (0);
		nbr = nbr * 10 + (shlvl[i] - '0');
		i++;
	}
	return (nbr);
}
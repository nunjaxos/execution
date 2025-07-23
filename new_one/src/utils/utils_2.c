#include "../../include/executor.h"

void	error_display_norm(t_cmd *cmd)
{
	if (cmd->access == NOT_AVAIL)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd->full_cmd[0], 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		free_data(get_data());
		exit(127);
	}
	else if (cmd->access == NOT_EXEC)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd->full_cmd[0], 2);
		ft_putstr_fd(": Permission denied\n", 2);
		free_data(get_data());
		exit(126);
	}
}

void	error_display(t_cmd *cmd)
{
	if (cmd->access != IS_EXEC)
	{
		if (cmd->access == NULL_CMD)
		{
			free_data(get_data()),
			exit(0);
		}
		if (cmd->access == CMD_NOT_FOUND)
		{
			ft_putstr_fd(cmd->full_cmd[0], 2);
			ft_putstr_fd(" : command not found\n", 2);
			free_data(get_data());
			exit(127);
		}
		else if (cmd->access == IS_A_DIR)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd->full_cmd[0], 2);
			ft_putstr_fd(": Is a directory\n", 2);
			free_data(get_data());
			exit(126);
		}
		error_display_norm(cmd);
	}
}

char	**convert_path_to_array(t_env *env)
{
	char	**paths;
	t_env	*tmp;
	int		i;

	tmp = env;
	i = get_size(tmp);
	paths = (char **)ft_malloc(sizeof(char *) * (i + 1));
	if (!paths)
		return (NULL);
	tmp = env;
	i = 0;
	convert_path_to_arr_norm(tmp, paths, &i);
	paths[i] = NULL;
	return (paths);
}

void	convert_path_to_arr_norm(t_env *tmp, char **paths, int *i)
{
	char	*temp;

	while (tmp)
	{
		if (tmp->value)
		{
			temp = ft_strjoin(tmp->name, "=");
			temp = ft_strjoin_free(temp, tmp->value, 1);
			paths[(*i)] = ft_strdup(temp);
			ft_free(temp);
			*i += 1;
		}
		tmp = tmp->next;
	}
}

int	*get_lastpid(void)
{
	static int	pid;

	return (&pid);
}

void	pipe_utils(t_cmd *cmd, int status)
{
	if (cmd->pid != *get_lastpid())
		waitpid(cmd->pid, &status, 0);
	else if (cmd->pid == *get_lastpid())
	{
		waitpid(cmd->pid, &status, 0);
		if (WIFEXITED(status))
			get_data()->exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			get_data()->exit_status = WTERMSIG(status) + 128;
	}
}
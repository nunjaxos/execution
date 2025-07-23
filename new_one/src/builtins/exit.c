#include "../../include/executor.h"

void	num_norm(t_cmd *cmd, int *i, int *sign)
{
	*i = skip_whitespace(cmd->full_cmd[1], *i);
	if (cmd->full_cmd[1][*i] == '-' || cmd->full_cmd[1][*i] == '+')
	{
		if (cmd->full_cmd[1][*i] == '-')
			*sign = -1;
		*i += 1;
	}
}

long long	num(t_cmd *cmd, size_t min_limit, int last)
{
	size_t	test;
	int		sign;
	int		i;
	int		digit;

	check_init(&test, &i, &sign, &digit);
	num_norm(cmd, &i, &sign);
	while (cmd->full_cmd[1][i])
	{
		if (!ft_isdigit(cmd->full_cmd[1][i]))
			return (2);
		digit = cmd->full_cmd[1][i] - '0';
		if (sign == -1 && undeflow_check(min_limit, test, digit, last))
			return (2);
		else if (sign == 1 && overflow_check(test, digit))
			return (2);
		test = (test * 10) + (cmd->full_cmd[1][i] - '0');
		i++;
	}
	if (cmd->full_cmd[1][skip_whitespace(cmd->full_cmd[1], i)])
		return (2);
	return (test);
}

void	exit_norm(t_cmd *cmd, int *exit_status, pid_t pid)
{
	if (pid)
		ft_putstr_fd("exit\n", 1);
	if (cmd->count > 1)
	{
		if (!args_check(cmd))
		{
			ft_putstr_fd("minishell: exit: ", 2);
			ft_putstr_fd(cmd->full_cmd[1], 2);
			ft_putstr_fd(": numeric argument required\n", 2);
			*exit_status = 2;
		}
		else if (cmd->count == 2)
			*exit_status = ft_atoi(cmd->full_cmd[1]);
		else
		{
			ft_putstr_fd("minishell: exit: too many arguments\n", 2);
			*exit_status = 1;
		}
	}
	else
		*exit_status = get_data()->exit_status;
}

int	ft_exit(t_cmd *cmd, pid_t pid)
{
	int	exit_status;

	exit_status = 0;
	exit_norm(cmd, &exit_status, pid);
	if (cmd->count > 2)
	{
		if (!args_check(cmd))
			exit_status = 2;
		else
		{
			check_for_child(pid, 1);
			return (1);
		}
	}
	check_for_child(pid, exit_status);
	free_data(get_data());
	exit(exit_status);
}

// int	main(int argc, char **argv, char **envp)
// {
// 	t_data	data;
// 	t_cmd	cmd;

// 	(void)argc;
// 	(void)argv;

// 	ft_bzero(&data, sizeof(t_data));
// 	init_env_list(envp, &data);

// 	char *args[] = {"exit", "1", "2", NULL};

// 	cmd.full_cmd = args;
// 	cmd.count = 2;  // important: count your args
// 	cmd.pid = 1;

// 	int exit_code = ft_exit(&cmd, cmd.pid);

// 	printf("ft_exit returned %d\n", exit_code);
// 	return exit_code;  // <-- return ft_exit code here
// }

#include "../../include/executor.h"

int	check_for_doubles(char *args)
{
	int	i;

	i = 0;
	if (args[i] == '-')
		i++;
	while (args[i])
	{
		if (args[i] != 'n')
			return (1);
		i++;
	}
	return (0);
}

void	ft_echo(t_cmd *cmd, pid_t pid)
{
	int	i;

	i = 1;
	while ((cmd->full_cmd[i] && (!check_for_doubles(cmd->full_cmd[i]))))
		i++;
	while (cmd->full_cmd[i])
	{
		if (!cmd->full_cmd[i])
			break ;
		ft_putstr_fd(cmd->full_cmd[i], 1);
		if (cmd->full_cmd[i + 1])
			ft_putstr_fd(" ", 1);
		i++;
	}
	if (cmd->full_cmd[1] && !check_for_doubles(cmd->full_cmd[1]))
	{
		check_for_child(pid, 0);
		return ;
	}
	ft_putstr_fd("\n", 1);
	check_for_child(pid, 0);
}

// int main(int argc, char **argv)
// {
//     t_cmd   cmd;
//     t_data  data;
//     pid_t   pid = 12345; // dummy pid
//     if (argc < 2)
//     {
//         printf("Usage: %s [args...]\n", argv[0]);
//         return 1;
//     }
//     char **full_cmd = malloc(sizeof(char *) * (argc + 1));
//     if (!full_cmd) 
//         return 1; // malloc failure
//     full_cmd[0] = "echo";  // pretend the command is "echo"
//     for (int i = 1; i < argc; i++)
//         full_cmd[i] = argv[i];
//     full_cmd[argc] = NULL;

//     cmd.full_cmd = full_cmd;
//     data.exit_status = 0;
//     ft_echo(&cmd, pid);
//     printf("\nExit status: %d\n", data.exit_status);
//     free(full_cmd);
//     return 0;
// }

#include "../../include/executor.h"

void	exit_status(char *failed, int code)
{
	ft_printf("%s: %s\n", failed, strerror(code));
	free_data(get_data());
	exit(1);
}

void	one_cmd_norm(t_cmd *cmd, t_data *data, pid_t pid)
{
	if (pid == -1)
		exit_status("fork", -1);
	if (pid == 0)
	{
		if (cmd->in_file != 0)
			dup2(cmd->in_file, 0);
		if (cmd->out_file != 1)
			dup2(cmd->out_file, 1);
		// test_redirection_fd(cmd);
		error_display(cmd);
		if (check_builtins(data, cmd, pid))
		{
			free_data(data);
			exit(0);
		}
		execve(cmd->full_cmd[0], cmd->full_cmd, convert_path_to_array(data->n_env));
		perror("execve");
		exit(255);
	}
}

void	exec_one_command(t_cmd *cmd, t_data *data)
{
	pid_t	pid;
	int		status;

	g_sigchild = 1;
	cmd->pid = 1;
	if (before_child(cmd))
		return ;
	pid = fork();
	signal(SIGQUIT, sigquit_handler);
	one_cmd_norm(cmd, data, pid);
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		data->exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		data->exit_status = WTERMSIG(status) + 128;
	g_sigchild = 0;
	signal(SIGQUIT, sigquit_handler);
}

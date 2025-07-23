#include "minishell.h"

t_env	*g_envp = NULL;

char	*ft_strjoin3(const char *s1, const char *s2, const char *s3)
{
	char	*joined;
	size_t	len1;
	size_t	len2;
	size_t	len3;

	if (!s1 || !s2 || !s3)
		return (NULL);
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	len3 = ft_strlen(s3);
	joined = malloc(len1 + len2 + len3 + 1);
	if (!joined)
		return (NULL);
	ft_strlcpy(joined, s1, len1 + 1);
	ft_strlcat(joined, s2, len1 + len2 + 1);
	ft_strlcat(joined, s3, len1 + len2 + len3 + 1);
	return (joined);
}

void	free_str_array(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return ;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

// ======================== ENVIRONMENT UTILS ============================ //

t_env	*create_env_node(const char *env)
{
	t_env	*node;
	char	*sep;

	if (!env)
		return (NULL);
	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	sep = strchr(env, '=');
	if (!sep)
	{
		node->name = ft_strdup(env);
		if (!node->name)
			return (free(node), NULL);
		node->value = NULL;
		node->next = NULL;
		return (node);
	}
	else
		node->name = ft_strndup(env, sep - env);
	if (!node->name)
		return (free(node), NULL);
	node->value = strdup(sep + 1);
	if (!node->value)
		return (free(node->name), free(node), NULL);
	node->next = NULL;
	return (node);
}

void	add_env_back(t_env **lst, t_env *new_node)
{
	t_env	*temp;

	if (!lst || !new_node)
		return ;
	if (!*lst)
	{
		*lst = new_node;
		return ;
	}
	temp = *lst;
	while (temp->next)
		temp = temp->next;
	temp->next = new_node;
}

void	init_env_list(char **envp)
{
	int		i;
	t_env	*node;

	i = 0;
	if (!envp)
		return ;
	while (envp[i])
	{
		node = create_env_node(envp[i]);
		if (node)
			add_env_back(&g_envp, node);
		i++;
	}
}

void	free_env_list(t_env *env)
{
	t_env	*tmp;

	while (env)
	{
		tmp = env;
		env = env->next;
		if (tmp->name)
			free(tmp->name);
		if (tmp->value)
			free(tmp->value);
		free(tmp);
	}
}

char	*get_env_value(char *key)
{
	t_env	*tmp;

	tmp = g_envp;
	if (!key)
		return (NULL);
	while (tmp)
	{
		if (tmp->name && !ft_strcmp(tmp->name, key))
			return (tmp->value);
		tmp = tmp->next;
	}
	return (NULL);
}

static char	*search_in_paths(char **paths, char *cmd)
{
	char	*full;
	int		i;

	i = 0;
	while (paths[i])
	{
		full = ft_strjoin3(paths[i], "/", cmd);
		if (full && access(full, X_OK) == 0)
			return (full);
		if (full)
			free(full);
		i++;
	}
	return (NULL);
}

char	*get_cmd_path(char *cmd)
{
	char	**paths;
	char	*env_path;
	char	*result;

	if (!cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	env_path = get_env_value("PATH");
	if (!env_path)
		return (ft_strdup(cmd));
	paths = ft_split(env_path, ':');
	if (!paths)
		return (NULL);
	result = search_in_paths(paths, cmd);
	free_str_array(paths);
	return (result);
}

	//env_norm_start
static int	count_valid_env_vars(t_env *env)
{
	int		count;

	count = 0;
	while (env)
	{
		if (env->name && env->value)
			count++;
		env = env->next;
	}
	return (count);
}

static void	cleanup_array_on_failure(char **arr, int i)
{
	while (--i >= 0)
		free(arr[i]);
	free(arr);
}

static char	*create_env_string(char *name, char *value)
{
	char	*tmp;
	char	*result;

	tmp = ft_strjoin(name, "=");
	if (!tmp)
		return (NULL);
	result = ft_strjoin(tmp, value);
	free(tmp);
	return (result);
}

char	**env_to_array(t_env *env)
{
	int		count;
	char	**arr;
	int		i;

	i = 0;
	count = count_valid_env_vars(env);
	arr = malloc(sizeof(char *) * (count + 1));
	if (!arr)
		return (NULL);
	while (env && i < count)
	{
		if (env->name && env->value)
		{
			arr[i] = create_env_string(env->name, env->value);
			if (!arr[i])
			{
				cleanup_array_on_failure(arr, i);
				return (NULL);
			}
			i++;
		}
		env = env->next;
	}
	arr[i] = NULL;
	return (arr);
}

	//env_norm_end
// ======================== BUILT-IN COMMANDS ============================ //

int	builtin_pwd(void)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)))
	{
		printf("%s\n", cwd);
		return (0);
	}
	perror("pwd");
	return (1);
}

int	builtin_echo(char **args)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	if (!args || !args[0])
		return (1);
	if (args[1] && ft_strcmp(args[1], "-n") == 0)
	{
		newline = 0;
		i++;
	}
	while (args[i])
	{
		printf("%s", args[i]);
		if (args[i + 1])
			printf(" ");
		i++;
	}
	if (newline)
		printf("\n");
	return (0);
}

int	builtin_exit(char **args)
{
	int	code;

	code = 0;
	if (args && args[1])
		code = ft_atoi(args[1]);
	free_env_list(g_envp);
	exit(code);
}

int	builtin_env(void)
{
	t_env	*tmp;

	tmp = g_envp;
	while (tmp)
	{
		if (tmp->name && tmp->value)
			printf("%s=%s\n", tmp->name, tmp->value);
		tmp = tmp->next;
	}
	return (0);
}

/*the export logic */
/* ---------------------- DISPLAY FUNCTIONS ---------------------- */

static void	print_exported_vars(void)
{
	t_env	*tmp;

	tmp = g_envp;
	while (tmp)
	{
		if (tmp->value)
			printf("declare -x %s=\"%s\"\n", tmp->name, tmp->value);
		else
			printf("declare -x %s\n", tmp->name);
		tmp = tmp->next;
	}
}

/* ---------------------- VARIABLE UPDATE FUNCTIONS ---------------------- */
static int	update_existing_var(t_env *node, char *new_value)
{
	if (!node || !new_value)
		return (0);
	if (node->value)
		free(node->value);
	node->value = ft_strdup(new_value);
	return (1);
}

static int	create_new_var(char *name, char *value)
{
	t_env	*new_node;
	char	*full_var;

	if (!name)
		return (0);
	if (!value)
		full_var = ft_strdup(name);
	else
		full_var = ft_strjoin3(name, "=", value);
	if (!full_var)
		return (0);
	new_node = create_env_node(full_var);
	free(full_var);
	if (!new_node)
		return (0);
	add_env_back(&g_envp, new_node);
	return (1);
}

/* ---------------------- ARGUMENT PROCESSING ---------------------- */

static int	process_var_assignment(char *arg)
{
	char	*sep;
	t_env	*tmp;

	sep = ft_strchr(arg, '=');
	if (!sep)
		return (create_new_var(arg, NULL));
	*sep = '\0';
	tmp = g_envp;
	while (tmp)
	{
		if (tmp->name && !ft_strcmp(tmp->name, arg))
		{
			*sep = '=';
			return (update_existing_var(tmp, sep + 1));
		}
		tmp = tmp->next;
	}
	*sep = '=';
	return (create_new_var(arg, sep + 1));
}

/* ---------------------- MAIN EXPORT FUNCTION ---------------------- */
int	builtin_export(char **args)
{
	int		i;

	if (!args || !args[0])
		return (1);
	if (!args[1])
	{
		print_exported_vars();
		return (0);
	}
	i = 1;
	while (args[i])
	{
		if (!process_var_assignment(args[i]))
			return (1);
		i++;
	}
	return (0);
}

/*the end of it */
static int	unset_single_var(char *var_name)
{
	t_env	*tmp;
	t_env	*prev;

	tmp = g_envp;
	prev = NULL;
	while (tmp)
	{
		if (tmp->name && !ft_strcmp(tmp->name, var_name))
		{
			if (prev)
				prev->next = tmp->next;
			else
				g_envp = tmp->next;
			if (tmp->name)
				free(tmp->name);
			if (tmp->value)
				free(tmp->value);
			free(tmp);
			return (1);
		}
		prev = tmp;
		tmp = tmp->next;
	}
	return (0);
}

int	builtin_unset(char **args)
{
	int		i;

	if (!args || !args[0])
		return (1);
	i = 1;
	while (args[i])
	{
		unset_single_var(args[i]);
		i++;
	}
	return (0);
}

int	builtin_cd(char **args)
{
	char	*home;

	if (!args || !args[0])
		return (1);
	if (!args[1])
	{
		home = get_env_value("HOME");
		if (!home)
			return (1);
		if (chdir(home) != 0)
		{
			perror("cd");
			return (1);
		}
	}
	else if (chdir(args[1]) != 0)
	{
		perror("cd");
		return (1);
	}
	return (0);
}

int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	return (!ft_strcmp(cmd, "cd") || !ft_strcmp(cmd, "echo") || !ft_strcmp(cmd,
			"pwd") || !ft_strcmp(cmd, "export") || !ft_strcmp(cmd, "unset")
		|| !ft_strcmp(cmd, "env") || !ft_strcmp(cmd, "exit"));
}

int	exec_builtin(t_cmd *cmd)
{
	if (!cmd || !cmd->full_cmd || !cmd->full_cmd[0])
		return (1);
	if (!ft_strcmp(cmd->full_cmd[0], "echo"))
		return (builtin_echo(cmd->full_cmd));
	if (!ft_strcmp(cmd->full_cmd[0], "cd"))
		return (builtin_cd(cmd->full_cmd));
	if (!ft_strcmp(cmd->full_cmd[0], "pwd"))
		return (builtin_pwd());
	if (!ft_strcmp(cmd->full_cmd[0], "export"))
		return (builtin_export(cmd->full_cmd));
	if (!ft_strcmp(cmd->full_cmd[0], "unset"))
		return (builtin_unset(cmd->full_cmd));
	if (!ft_strcmp(cmd->full_cmd[0], "env"))
		return (builtin_env());
	if (!ft_strcmp(cmd->full_cmd[0], "exit"))
		return (builtin_exit(cmd->full_cmd));
	return (1);
}

int	execute_builtin_command(t_cmd *cmd)
{
	int	saved_stdin;
	int	saved_stdout;
	int	ret;

	saved_stdin = -1;
	saved_stdout = -1;
	if (cmd->in_file != STDIN_FILENO)
	{
		saved_stdin = dup(STDIN_FILENO);
		dup2(cmd->in_file, STDIN_FILENO);
	}
	if (cmd->out_file != STDOUT_FILENO)
	{
		saved_stdout = dup(STDOUT_FILENO);
		dup2(cmd->out_file, STDOUT_FILENO);
	}
	ret = exec_builtin(cmd);
	if (saved_stdin != -1)
	{
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdin);
	}
	if (saved_stdout != -1)
	{
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdout);
	}
	return (ret);
}

// Function 2: Setup child process with I/O redirection and execute command
void	setup_and_exec_child(t_cmd *cmd, char **envp)
{
	char	*path;

	default_signals();
	if (cmd->in_file != STDIN_FILENO)
	{
		dup2(cmd->in_file, STDIN_FILENO);
		close(cmd->in_file);
	}
	if (cmd->out_file != STDOUT_FILENO)
	{
		dup2(cmd->out_file, STDOUT_FILENO);
		close(cmd->out_file);
	}
	path = get_cmd_path(cmd->full_cmd[0]);
	if (!path)
	{
		fprintf(stderr, "%s: command not found\n", cmd->full_cmd[0]);
		free_str_array(envp);
		exit(127);
	}
	execve(path, cmd->full_cmd, envp);
	perror(path);
	free(path);
	free_str_array(envp);
	exit(127);
}

// Function 3: Handle fork and child process execution
int	fork_and_execute(t_cmd *cmd, char **envp)
{
	pid_t	pid;
	int		status;
	int		ret;

	pid = fork();
	if (pid == 0)
	{
		setup_and_exec_child(cmd, envp);
	}
	else if (pid < 0)
	{
		perror("fork");
		return (1);
	}
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status))
		ret = 128 + WTERMSIG(status);
	else if (WIFEXITED(status))
		ret = WEXITSTATUS(status);
	else
		ret = 1;
	return (ret);
}

// Function 4: Main execution function (orchestrates the process)
int	execute_single_command(t_cmd *cmd)
{
	char	**envp;
	int		ret;

	if (!cmd || !cmd->full_cmd || !cmd->full_cmd[0])
		return (1);
	if (is_builtin(cmd->full_cmd[0]))
		return (execute_builtin_command(cmd));
	set_child_running();
	envp = env_to_array(g_envp);
	if (!envp)
	{
		set_child_finished();
		return (1);
	}
	ret = fork_and_execute(cmd, envp);
	set_child_finished();
	free_str_array(envp);
	return (ret);
}

// Function 1: Count commands in pipeline
int	count_commands(t_cmd *cmd)
{
	int	cmd_count;

	cmd_count = 0;
	while (cmd)
	{
		if (cmd->full_cmd && cmd->full_cmd[0])
			cmd_count++;
		cmd = cmd->next;
	}
	return (cmd_count);
}

// Function 2: Handle input redirection for child process
void	handle_input_redirection(t_cmd *cmd, int prev_fd)
{
	if (cmd->in_file != STDIN_FILENO)
	{
		dup2(cmd->in_file, STDIN_FILENO);
		close(cmd->in_file);
	}
	else if (prev_fd != -1)
	{
		dup2(prev_fd, STDIN_FILENO);
		close(prev_fd);
	}
}

// Function 3: Handle output redirection for child process
void	handle_output_redirection(t_cmd *cmd, int *pipefd)
{
	if (cmd->out_file != STDOUT_FILENO)
	{
		dup2(cmd->out_file, STDOUT_FILENO);
		close(cmd->out_file);
	}
	else if (cmd->next)
	{
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
	}
}

// Function 4: Execute command in child process
void	execute_child_command(t_cmd *cmd, char **envp)
{
	char	*path;
	int		ret;

	if (is_builtin(cmd->full_cmd[0]))
	{
		ret = exec_builtin(cmd);
		free_str_array(envp);
		exit(ret);
	}
	path = get_cmd_path(cmd->full_cmd[0]);
	if (!path)
	{
		fprintf(stderr, "%s: command not found\n", cmd->full_cmd[0]);
		free_str_array(envp);
		exit(127);
	}
	execve(path, cmd->full_cmd, envp);
	perror(path);
	free(path);
	free_str_array(envp);
	exit(127);
}

// Function 5: Handle child process creation and execution
int	handle_child_process(t_cmd *cmd, int *pipefd, int prev_fd, char **envp)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		default_signals();
		handle_input_redirection(cmd, prev_fd);
		handle_output_redirection(cmd, pipefd);
		if (cmd->next)
			close(pipefd[0]);
		execute_child_command(cmd, envp);
	}
	else if (pid < 0)
	{
		perror("fork");
		return (-1);
	}
	return (0);
}

// Function 1: Initialize pipeline execution
char	**init_pipeline(t_data *data)
{
	char	**envp;

	if (!data || !data->head)
		return (NULL);
	if (count_commands(data->head) == 1)
		return ((char **)1);
	set_child_running();
	envp = env_to_array(g_envp);
	if (!envp)
	{
		set_child_finished();
		return (NULL);
	}
	return (envp);
}

int	execute_one_pipeline_step(t_cmd *cmd, char **envp,
	int *prev_fd, int pipefd[2])
{
	if (cmd->next && pipe(pipefd) < 0)
	{
		perror("pipe");
		if (*prev_fd != -1)
			close(*prev_fd);
		return (-1);
	}
	if (handle_child_process(cmd, pipefd, *prev_fd, envp) == -1)
	{
		if (*prev_fd != -1)
			close(*prev_fd);
		if (cmd->next)
		{
			close(pipefd[0]);
			close(pipefd[1]);
		}
		return (-1);
	}
	if (*prev_fd != -1)
		close(*prev_fd);
	if (cmd->next)
	{
		close(pipefd[1]);
		*prev_fd = pipefd[0];
	}
	return (0);
}

// Function 2: Execute pipeline commands loop
int	execute_pipeline_commands(t_cmd *cmd, char **envp)
{
	int		pipefd[2];
	int		prev_fd;

	prev_fd = -1;
	while (cmd)
	{
		if (!cmd->full_cmd || !cmd->full_cmd[0])
		{
			cmd = cmd->next;
			continue ;
		}
		if (execute_one_pipeline_step(cmd, envp, &prev_fd, pipefd) == -1)
			return (-1);
		cmd = cmd->next;
	}
	if (prev_fd != -1)
		close(prev_fd);
	return (0);
}

// Function 3: Wait for children and cleanup
int	wait_and_cleanup(char **envp)
{
	int	status;
	int	last_exit_status;

	last_exit_status = 0;
	while (wait(&status) > 0)
	{
		if (WIFSIGNALED(status))
			last_exit_status = 128 + WTERMSIG(status);
		else if (WIFEXITED(status))
			last_exit_status = WEXITSTATUS(status);
	}
	set_child_finished();
	free_str_array(envp);
	return (last_exit_status);
}

// Main function: Execute pipeline (orchestrates the 3 parts)
int	execute_pipeline(t_data *data)
{
	char	**envp;

	envp = init_pipeline(data);
	if (!envp)
		return (1);
	if (envp == (char **)1)
		return (execute_single_command(data->head));
	if (execute_pipeline_commands(data->head, envp) == -1)
	{
		free_str_array(envp);
		set_child_finished();
		return (1);
	}
	return (wait_and_cleanup(envp));
}

// ============================ SIGNALS ============================ //

int	g_sigchild = 0;

static int	*g_last_exit_code = NULL;

// Wrapper functions for signal handlers
void	sigint_wrapper(int signo)
{
	if (g_last_exit_code)
	{
		*g_last_exit_code = 130;
		if (signo == SIGINT && g_sigchild)
		{
			printf("\n");
			rl_on_new_line();
			rl_replace_line("", 0);
			g_sigchild = 0;
		}
		else
		{
			printf("\n");
			rl_on_new_line();
			rl_replace_line("", 0);
			rl_redisplay();
		}
	}
}

void	sigquit_wrapper(int signo)
{
	if (g_last_exit_code)
	{
		*g_last_exit_code = 131;
		if (signo == SIGQUIT && g_sigchild)
		{
			printf("Quit (core dumped)\n");
			rl_on_new_line();
			rl_replace_line("", 0);
			g_sigchild = 0;
		}
		else
		{
			rl_replace_line("", 0);
			rl_on_new_line();
			rl_redisplay();
		}
	}
}

// Function to set up signal handlers
void	handle_signals(int *last_exit_code)
{
	g_last_exit_code = last_exit_code;
	signal(SIGINT, sigint_wrapper);
	signal(SIGQUIT, sigquit_wrapper);
}

// Function to ignore signals (useful during command execution)
void	ignore_signals(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

// Function to restore default signal behavior (for child processes)
void	default_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

void	set_child_running(void)
{
	g_sigchild = 1;
}

void	set_child_finished(void)
{
	g_sigchild = 0;
}
#include "minishell.h"

char	*ft_strjoin3(const char *s1, const char *s2, const char *s3)
{
	char	*joined;

	size_t len1, len2, len3;
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

t_env    *create_env_node(const char *env)
{
    t_env    *node;
    char    *sep;

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

void    add_env_back(t_env **lst, t_env *new_node)
{
    t_env    *temp;

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

// Modified to return the created environment list instead of using global
t_env    *init_env_list(char **envp)
{
    int        i;
    t_env    *node;
    t_env    *env_list;

    i = 0;
    env_list = NULL;
    if (!envp)
        return (NULL);
    while (envp[i])
    {
        node = create_env_node(envp[i]);
        if (node)
            add_env_back(&env_list, node);
        i++;
    }
    return (env_list);
}

void    free_env_list(t_env *env)
{
    t_env    *tmp;

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

// Modified to take env_list as parameter
char    *get_env_value(t_env *env_list, char *key)
{
    t_env    *tmp;

    tmp = env_list;
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
static char *search_in_paths(char **paths, char *cmd)
{
    char *full;
    int i = 0;
    
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

// Modified to take env_list as parameter
char *get_cmd_path(char *cmd, t_env *env_list)
{
    char **paths;
    char *env_path;
    char *result;
    
    if (!cmd)
        return (NULL);
    if (ft_strchr(cmd, '/'))
        return (ft_strdup(cmd));
    env_path = get_env_value(env_list, "PATH");
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
static int count_valid_env_vars(t_env *env)
{
    int count = 0;
    
    while (env)
    {
        if (env->name && env->value)
            count++;
        env = env->next;
    }
    return count;
}

static void cleanup_array_on_failure(char **arr, int i)
{
    while (--i >= 0)
        free(arr[i]);
    free(arr);
}

static char *create_env_string(char *name, char *value)
{
    char *tmp;
    char *result;
    
    tmp = ft_strjoin(name, "=");
    if (!tmp)
        return (NULL);
    result = ft_strjoin(tmp, value);
    free(tmp);
    return (result);
}

char **env_to_array(t_env *env)
{
    int count, i = 0;
    char **arr;
    
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

int    builtin_env(t_env *env_list)
{
    t_env    *tmp;

    tmp = env_list;
    while (tmp)
    {
        if (tmp->name && tmp->value)
            printf("%s=%s\n", tmp->name, tmp->value);
        tmp = tmp->next;
    }
    return (0);
}

// Modified to take env_list as parameter
int	builtin_exit(char **args, t_env *env_list)
{
	int	code;

	code = 0;
	if (args && args[1])
		code = ft_atoi(args[1]);
	free_env_list(env_list);
	exit(code);
}

/*the export logic */
/* ---------------------- DISPLAY FUNCTIONS ---------------------- */

// Modified to take env_list as parameter
static void print_exported_vars(t_env *env_list)
{
    t_env *tmp = env_list;
    
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

static int update_existing_var(t_env *node, char *new_value)
{
    if (!node || !new_value)
        return (0);
    
    if (node->value)
        free(node->value);
    node->value = ft_strdup(new_value);
    return (1);
}

// Modified to take env_list as parameter
static int create_new_var(char *name, char *value, t_env **env_list)
{
	t_env *new_node;
	char *full_var;

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

	add_env_back(env_list, new_node);
	return (1);
}

/* ---------------------- ARGUMENT PROCESSING ---------------------- */

// Modified to take env_list as parameter
static int process_var_assignment(char *arg, t_env **env_list)
{
    char *sep = ft_strchr(arg, '=');
    t_env *tmp;

    if (!sep)
        return (create_new_var(arg, NULL, env_list));
    
    *sep = '\0';  // Temporarily split at '='
    tmp = *env_list;
    while (tmp)
    {
        if (tmp->name && !ft_strcmp(tmp->name, arg))
        {
            *sep = '=';
            return update_existing_var(tmp, sep + 1);
        }
        tmp = tmp->next;
    }
    *sep = '=';
    return create_new_var(arg, sep + 1, env_list);
}

/* ---------------------- MAIN EXPORT FUNCTION ---------------------- */

// Modified to take env_list as parameter
int builtin_export(char **args, t_env **env_list)
{
	int		i;

    if (!args || !args[0])
        return (1);
    
    if (!args[1])
    {
        print_exported_vars(*env_list);
        return (0);
    }
	i = 1;
    while (args[i])
    {
        if (!process_var_assignment(args[i], env_list))
            return (1);
        i++;
    }
    return (0);
}
/*the end of it */

// Modified to take env_list as parameter
static int unset_single_var(char *var_name, t_env **env_list)
{
    t_env *tmp, *prev;
    
    tmp = *env_list;
    prev = NULL;
    
    while (tmp)
    {
        if (tmp->name && !ft_strcmp(tmp->name, var_name))
        {
            if (prev)
                prev->next = tmp->next;
            else
                *env_list = tmp->next;
            if (tmp->name)
                free(tmp->name);
            if (tmp->value)
                free(tmp->value);
            free(tmp);
            
            return (1); // Variable found and removed
        }
        prev = tmp;
        tmp = tmp->next;
    }
    return (0); // Variable not found
}

// Modified to take env_list as parameter
int builtin_unset(char **args, t_env **env_list)
{
    int i;
    
    if (!args || !args[0])
        return (1);
    
    i = 1;
    while (args[i])
    {
        unset_single_var(args[i], env_list);
        i++;
    }
    
    return (0);
}

// Modified to take env_list as parameter
int	builtin_cd(char **args, t_env *env_list)
{
	char	*home;

	if (!args || !args[0])
		return (1);
	if (!args[1])
	{
		home = get_env_value(env_list, "HOME");
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

// Modified to take env_list as parameter
int	exec_builtin(t_cmd *cmd, t_env **env_list)
{
	if (!cmd || !cmd->full_cmd || !cmd->full_cmd[0])
		return (1);
	if (!ft_strcmp(cmd->full_cmd[0], "echo"))
		return (builtin_echo(cmd->full_cmd));
	if (!ft_strcmp(cmd->full_cmd[0], "cd"))
		return (builtin_cd(cmd->full_cmd, *env_list));
	if (!ft_strcmp(cmd->full_cmd[0], "pwd"))
		return (builtin_pwd());
	if (!ft_strcmp(cmd->full_cmd[0], "export"))
		return (builtin_export(cmd->full_cmd, env_list));
	if (!ft_strcmp(cmd->full_cmd[0], "unset"))
		return (builtin_unset(cmd->full_cmd, env_list));
	if (!ft_strcmp(cmd->full_cmd[0], "env"))
		return (builtin_env(*env_list));
	if (!ft_strcmp(cmd->full_cmd[0], "exit"))
		return (builtin_exit(cmd->full_cmd, *env_list));
	return (1);
}

// Modified to take env_list as parameter
int	execute_builtin_command(t_cmd *cmd, t_env **env_list)
{
	int	saved_stdin = -1, saved_stdout = -1;
	int	ret;

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
	ret = exec_builtin(cmd, env_list);
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
// Modified to take env_list as parameter
void	setup_and_exec_child(t_cmd *cmd, char **envp, t_env *env_list)
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
	path = get_cmd_path(cmd->full_cmd[0], env_list);
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
// Modified to take env_list as parameter
int	fork_and_execute(t_cmd *cmd, char **envp, t_env *env_list)
{
	pid_t	pid;
	int		status;
	int		ret;

	pid = fork();
	if (pid == 0)
	{
		setup_and_exec_child(cmd, envp, env_list);
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
// Modified to take env_list as parameter
int	execute_single_command(t_cmd *cmd, t_env **env_list)
{
	char	**envp;
	int		ret;

	if (!cmd || !cmd->full_cmd || !cmd->full_cmd[0])
		return (1);
	if (is_builtin(cmd->full_cmd[0]))
		return (execute_builtin_command(cmd, env_list));
	set_child_running();
	envp = env_to_array(*env_list);
	if (!envp)
	{
		set_child_finished();
		return (1);
	}
	ret = fork_and_execute(cmd, envp, *env_list);
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
// Modified to take env_list as parameter
void	execute_child_command(t_cmd *cmd, char **envp, t_env **env_list)
{
	char	*path;
	int		ret;

	if (is_builtin(cmd->full_cmd[0]))
	{
		ret = exec_builtin(cmd, env_list);
		free_str_array(envp);
		exit(ret);
	}
	path = get_cmd_path(cmd->full_cmd[0], *env_list);
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
// Modified to take env_list as parameter
int	handle_child_process(t_cmd *cmd, int *pipefd, int prev_fd, char **envp, t_env **env_list)
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
		execute_child_command(cmd, envp, env_list);
	}
	else if (pid < 0)
	{
		perror("fork");
		return (-1);
	}
	return (0);
}

// Function 1: Initialize pipeline execution
// Modified to take env_list as parameter
char	**init_pipeline(t_data *data, t_env *env_list)
{
	char	**envp;

	if (!data || !data->head)
		return (NULL);
	if (count_commands(data->head) == 1)
		return ((char **)1); // Special return value for single command
	set_child_running();
	envp = env_to_array(env_list);
	if (!envp)
	{
		set_child_finished();
		return (NULL);
	}
	return (envp);
}

// Modified to take env_list as parameter
int	execute_one_pipeline_step(t_cmd *cmd, char **envp, int *prev_fd, int pipefd[2], t_env **env_list)
{
	if (cmd->next && pipe(pipefd) < 0)
	{
		perror("pipe");
		if (*prev_fd != -1)
			close(*prev_fd);
		return (-1);
	}
	if (handle_child_process(cmd, pipefd, *prev_fd, envp, env_list) == -1)
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
// Modified to take env_list as parameter
int	execute_pipeline_commands(t_cmd *cmd, char **envp, t_env **env_list)
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
		if (execute_one_pipeline_step(cmd, envp, &prev_fd, pipefd, env_list) == -1)
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
// Modified to take env_list as parameter
int	execute_pipeline(t_data *data, t_env **env_list)
{
	char	**envp;

	envp = init_pipeline(data, *env_list);
	if (!envp)
		return (1);
	if (envp == (char **)1)
		return (execute_single_command(data->head, env_list));
	if (execute_pipeline_commands(data->head, envp, env_list) == -1)
	{
		free_str_array(envp);
		set_child_finished();
		return (1);
	}
	return (wait_and_cleanup(envp));
}

// ============================ SIGNALS ============================ //

int	g_sigchild = 0;

static int *g_last_exit_code = NULL;

// Wrapper functions for signal handlers
void sigint_wrapper(int signo)
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

void sigquit_wrapper(int signo)
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
void handle_signals(int *last_exit_code)
{
    g_last_exit_code = last_exit_code;
    signal(SIGINT, sigint_wrapper);
    signal(SIGQUIT, sigquit_wrapper);
}

// Function to ignore signals (useful during command execution)
void ignore_signals(void)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
}

// Function to restore default signal behavior (for child processes)
void default_signals(void)
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
}

void set_child_running(void)
{
    g_sigchild = 1;
}

void set_child_finished(void)
{
    g_sigchild = 0;
}
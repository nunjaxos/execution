#include "../../include/executor.h"

int	execute_commands(t_data *data)
{
	if (count_all_commands(data->head) == 1)
	{
		update_last_cmd(get_data());
		exec_one_command(data->head[0], data);
	}
	else
	{
		update_last_cmd(get_data());
		create_pipe(data->head, count_all_commands(data->head));
	}

	return (0);
}
		//test

void free_char_array(char **arr)
{
    int i = 0;
    if (!arr)
        return;
    while (arr[i])
    {
        free(arr[i]);
        i++;
    }
    free(arr);
}

char *find_full_path(char *cmd, t_env *env)
{
    char *path_env = NULL;
    char **paths = NULL;
    char *full_path = NULL;
    struct stat sb;
    int i = 0;

    // Find PATH value in env linked list
    while (env)
    {
        if (!strcmp(env->name, "PATH"))
        {
            path_env = env->value;
            break;
        }
        env = env->next;
    }
    if (!path_env)
        return NULL;

    // Split PATH by ':'
    paths = ft_split(path_env, ':');
    if (!paths)
        return NULL;

    // Try each path + "/" + cmd
    while (paths[i])
    {
        full_path = ft_strjoin(paths[i], "/");
        full_path = ft_strjoin_free(full_path, cmd, 1); // custom function to concat and free first param
        if (stat(full_path, &sb) == 0 && (sb.st_mode & S_IXUSR))
        {
            // Free split array
            free_char_array(paths);
            return full_path; // Found executable
        }
        free(full_path);
        full_path = NULL;
        i++;
    }
    free_char_array(paths);
    return NULL;
}

t_env	*find_env(t_env *env, char *name)
{
	while (env)
	{
		if (ft_strcmp(env->name, name) == 0)
			return (env);
		env = env->next;
	}
	return (NULL);
}

// int	main(int argc, char **argv, char **envp)
// {
// 	(void)argc;
// 	(void)argv;

// 	// Allocate and initialize data
// 	t_data	*data = malloc(sizeof(t_data));
// 	if (!data)
// 		return (1);
// 	ft_bzero(data, sizeof(t_data));

// 	// Initialize environment list (if your logic depends on it)
// 	init_env_list(envp, data->n_env);

// 	// Create one command (e.g. "ls -la")
// 	t_cmd	*cmd = malloc(sizeof(t_cmd));
// 	if (!cmd)
// 		return (1);
// 	ft_bzero(cmd, sizeof(t_cmd));

// 	// Build the full command array
// 	// full_cmd[0] is the command name
// 	// full_cmd[1+] are arguments
// 	cmd->full_cmd = malloc(sizeof(char *) * 3);
// 	cmd->full_cmd[0] = ft_strdup("ls");
// 	cmd->full_cmd[1] = ft_strdup("-la");
// 	cmd->full_cmd[2] = NULL;

// 	// Standard input/output
// 	cmd->in_file = STDIN_FILENO;
// 	cmd->out_file = STDOUT_FILENO;

// 	// No next command (only one)
// 	cmd->next = NULL;

// 	// Point data->head to the command list
// 	data->head = &cmd;

// 	// Run the executor (this should fork + exec ls)
// 	execute_commands(data);

// 	// Cleanup (if your project has a proper free system, call it here)
// 	free_char_array(cmd->full_cmd);
// 	free(cmd);
// 	free(data);

// 	return (0);
// }

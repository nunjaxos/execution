// minishell_executor.c

#include "../include/minishell.h"

t_env *g_envp = NULL;

char *ft_strndup(const char *s, size_t n)
{
    size_t i;
    char *dup;

    dup = (char *)malloc(n + 1);
    if (!dup)
        return NULL;
    i = 0;
    while (i < n && s[i])
    {
        dup[i] = s[i];
        i++;
    }
    dup[i] = '\0';
    return dup;
}

int ft_strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}


char *ft_strjoin3(const char *s1, const char *s2, const char *s3)
{
    char *joined;
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

    return joined;
}

void free_str_array(char **arr)
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

// ======================== ENVIRONMENT UTILS ============================ //

t_env *create_env_node(const char *env)
{
    t_env *node;
    char *sep;

    if (!env)
        return (NULL);
        
    node = malloc(sizeof(t_env));
    if (!node)
        return (NULL);
        
    sep = strchr(env, '=');
    if (!sep)
        return (free(node), NULL);
        
    node->name = ft_strndup(env, sep - env);
    if (!node->name)
        return (free(node), NULL);
        
    node->value = strdup(sep + 1);
    if (!node->value)
        return (free(node->name), free(node), NULL);
        
    node->next = NULL;
    return (node);
}

void add_env_back(t_env **lst, t_env *new_node)
{
    t_env *temp;

    if (!lst || !new_node)
        return;
    if (!*lst)
    {
        *lst = new_node;
        return;
    }
    temp = *lst;
    while (temp->next)
        temp = temp->next;
    temp->next = new_node;
}

void init_env_list(char **envp)
{
    int i = 0;
    t_env *node;

    if (!envp)
        return;
        
    while (envp[i])
    {
        node = create_env_node(envp[i]);
        if (node)
            add_env_back(&g_envp, node);
        i++;
    }
}

void free_env_list(t_env *env)
{
    t_env *tmp;

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

char *get_env_value(char *key)
{
    t_env *tmp = g_envp;
    
    if (!key)
        return NULL;
        
    while (tmp)
    {
        if (tmp->name && !ft_strcmp(tmp->name, key))
            return tmp->value;
        tmp = tmp->next;
    }
    return NULL;
}

char *get_cmd_path(char *cmd)
{
    char **paths;
    char *env_path;
    char *full;
    int i;

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

    i = 0;
    while (paths[i])
    {
        full = ft_strjoin3(paths[i], "/", cmd);
        if (full && access(full, X_OK) == 0)
        {
            free_str_array(paths);
            return (full);
        }
        if (full)
            free(full);
        i++;
    }
    free_str_array(paths);
    return (NULL);
}

char **env_to_array(t_env *env)
{
    int count = 0, i = 0;
    char **arr;
    char *tmp;
    t_env *copy = env;

    while (copy) { count++; copy = copy->next; }
    
    arr = malloc(sizeof(char *) * (count + 1));
    if (!arr)
        return NULL;

    while (env && i < count)
    {
        if (env->name && env->value)
        {
            tmp = ft_strjoin(env->name, "=");
            if (tmp)
            {
                arr[i] = ft_strjoin(tmp, env->value);
                free(tmp);
                if (arr[i])
                    i++;
            }
        }
        env = env->next;
    }
    arr[i] = NULL;
    return arr;
}

// ======================== BUILT-IN COMMANDS ============================ //

int builtin_pwd(void)
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)))
    {
        printf("%s\n", cwd);
        return (0);
    }
    perror("pwd");
    return (1);
}

int builtin_echo(char **args)
{
    int i = 1;
    int newline = 1;

    if (!args)
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

int builtin_exit(char **args)
{
    int code = 0;
    if (args && args[1])
        code = ft_atoi(args[1]);
    exit(code);
}

int builtin_env(void)
{
    t_env *tmp = g_envp;
    while (tmp)
    {
        if (tmp->name && tmp->value)
            printf("%s=%s\n", tmp->name, tmp->value);
        tmp = tmp->next;
    }
    return (0);
}

int builtin_export(char **args)
{
    int i = 1;
    char *sep;
    t_env *tmp;
    char *concat, *full;
    t_env *new_node;
    
    if (!args)
        return (1);
        
    while (args[i])
    {
        sep = ft_strchr(args[i], '=');
        if (sep)
        {
            *sep = '\0';
            tmp = g_envp;
            
            // Find existing variable
            while (tmp)
            {
                if (tmp->name && !ft_strcmp(tmp->name, args[i]))
                {
                    if (tmp->value)
                        free(tmp->value);
                    tmp->value = ft_strdup(sep + 1);
                    break;
                }
                tmp = tmp->next;
            }
            
            // Create new variable if not found
            if (!tmp)
            {
                concat = ft_strjoin(args[i], "=");
                if (concat)
                {
                    full = ft_strjoin(concat, sep + 1);
                    if (full)
                    {
                        new_node = create_env_node(full);
                        if (new_node)
                            add_env_back(&g_envp, new_node);
                        free(full);
                    }
                    free(concat);
                }
            }
            *sep = '='; // Restore original string
        }
        i++;
    }
    return (0);
}

int builtin_unset(char **args)
{
    int i = 1;
    t_env *tmp, *prev;
    
    if (!args)
        return (1);
        
    while (args[i])
    {
        tmp = g_envp;
        prev = NULL;
        
        while (tmp)
        {
            if (tmp->name && !ft_strcmp(tmp->name, args[i]))
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
                break;
            }
            prev = tmp;
            tmp = tmp->next;
        }
        i++;
    }
    return (0);
}

int builtin_cd(char **args)
{
    char *home;
    
    if (!args)
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

int is_builtin(char *cmd)
{
    if (!cmd)
        return (0);
        
    return (!ft_strcmp(cmd, "cd") || !ft_strcmp(cmd, "echo") ||
        !ft_strcmp(cmd, "pwd") || !ft_strcmp(cmd, "export") ||
        !ft_strcmp(cmd, "unset") || !ft_strcmp(cmd, "env") ||
        !ft_strcmp(cmd, "exit"));
}

int exec_builtin(t_cmd *cmd)
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

// =========================== PIPE EXECUTION ============================ //

// Refactored pipeline execution functions (replace in your executor file)

static void setup_child_pipes(t_cmd *cmd, int prev_fd, int *pipefd)
{
    // Setup input redirection
    if (cmd->in_file != STDIN_FILENO)
        dup2(cmd->in_file, STDIN_FILENO);
    else if (prev_fd != -1)
        dup2(prev_fd, STDIN_FILENO);

    // Setup output redirection  
    if (cmd->out_file != STDOUT_FILENO)
        dup2(cmd->out_file, STDOUT_FILENO);
    else if (cmd->next)
        dup2(pipefd[1], STDOUT_FILENO);
}

static void close_child_pipes(int prev_fd, int *pipefd, t_cmd *cmd)
{
    if (prev_fd != -1) 
        close(prev_fd);
    if (cmd->next)
    {
        close(pipefd[0]);
        close(pipefd[1]);
    }
}

static int exec_child_process(t_cmd *cmd, char **envp)
{
    char *path;

    if (is_builtin(cmd->full_cmd[0]))
        exit(exec_builtin(cmd));

    path = get_cmd_path(cmd->full_cmd[0]);
    if (!path)
    {
        fprintf(stderr, "%s: command not found\n", cmd->full_cmd[0]);
        exit(127);
    }
    
    execve(path, cmd->full_cmd, envp);
    perror(path);
    free(path);
    exit(127);
}

static int create_child_process(t_cmd *cmd, int prev_fd, int *pipefd, char **envp)
{
    pid_t pid;

    pid = fork();
    if (pid == 0)
    {
        // Child process
        setup_child_pipes(cmd, prev_fd, pipefd);
        close_child_pipes(prev_fd, pipefd, cmd);
        exec_child_process(cmd, envp);
    }
    else if (pid < 0)
    {
        perror("fork");
        return (-1);
    }
    return (0);
}

static void cleanup_parent_pipes(int prev_fd, int *pipefd, t_cmd *cmd)
{
    if (prev_fd != -1)
        close(prev_fd);
    if (cmd->next)
        close(pipefd[1]);
}

static int wait_for_children(void)
{
    while (wait(NULL) > 0)
        ;
    return (0);
}

int execute_pipeline(t_data *data)
{
    t_cmd *cmd;
    int pipefd[2];
    int prev_fd = -1;
    char **envp;

    if (!data || !data->head)
        return (1);  
    envp = env_to_array(g_envp);
    if (!envp)
        return (1);
    cmd = data->head;
    while (cmd)
    {
        if (!cmd->full_cmd || !cmd->full_cmd[0])
        {
            cmd = cmd->next;
            continue;
        }
        if (cmd->next && pipe(pipefd) < 0)
        {
            perror("pipe");
            free_str_array(envp);
            return (1);
        }
        if (create_child_process(cmd, prev_fd, pipefd, envp) == -1)
        {
            free_str_array(envp);
            return (1);
        }
        cleanup_parent_pipes(prev_fd, pipefd, cmd);
        if (cmd->next)
            prev_fd = pipefd[0];
        cmd = cmd->next;
    }
    wait_for_children();
    free_str_array(envp);
    return (0);
}

// ============================ SIGNALS ============================ //

void handle_sigint(int sig)
{
    (void)sig;
    write(1, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

void handle_signals(void)
{
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
}
            //main functions
            t_cmd *create_simple_cmd(char *input)
{
    t_cmd *cmd;
    char **tokens;
    int i, token_count = 0;
    
    if (!input || !*input)
        return NULL;
        
    cmd = malloc(sizeof(t_cmd));
    if (!cmd)
        return NULL;
        
    // Initialize command structure
    cmd->full_cmd = NULL;
    cmd->in_file = STDIN_FILENO;
    cmd->out_file = STDOUT_FILENO;
    cmd->next = NULL;
    
    // Simple tokenization (split by spaces)
    tokens = ft_split(input, ' ');
    if (!tokens)
    {
        free(cmd);
        return NULL;
    }
    
    // Count tokens
    while (tokens[token_count])
        token_count++;
        
    if (token_count == 0)
    {
        free_str_array(tokens);
        free(cmd);
        return NULL;
    }
    
    // Allocate command array
    cmd->full_cmd = malloc(sizeof(char *) * (token_count + 1));
    if (!cmd->full_cmd)
    {
        free_str_array(tokens);
        free(cmd);
        return NULL;
    }
    
    // Copy tokens to command
    for (i = 0; i < token_count; i++)
    {
        cmd->full_cmd[i] = ft_strdup(tokens[i]);
        if (!cmd->full_cmd[i])
        {
            // Cleanup on error
            while (--i >= 0)
                free(cmd->full_cmd[i]);
            free(cmd->full_cmd);
            free_str_array(tokens);
            free(cmd);
            return NULL;
        }
    }
    cmd->full_cmd[token_count] = NULL;
    
    free_str_array(tokens);
    return cmd;
}

void free_cmd(t_cmd *cmd)
{
    int i;
    
    if (!cmd)
        return;
        
    if (cmd->full_cmd)
    {
        i = 0;
        while (cmd->full_cmd[i])
        {
            free(cmd->full_cmd[i]);
            i++;
        }
        free(cmd->full_cmd);
    }
    
    free(cmd);
}

void print_welcome(void)
{
    printf("\n");
    printf("====================================\n");
    printf("     MINISHELL TEST ENVIRONMENT     \n");
    printf("====================================\n");
    printf("Available commands:\n");
    printf("  Built-ins: cd, echo, pwd, export, unset, env, exit\n");
    printf("  External: any command in PATH\n");
    printf("  Type 'help' for this message\n");
    printf("  Type 'exit' or Ctrl+C to quit\n");
    printf("====================================\n\n");
}

void print_help(void)
{
    printf("\nMinishell Test Commands:\n");
    printf("------------------------\n");
    printf("Built-in commands:\n");
    printf("  pwd                    - print working directory\n");
    printf("  echo [text]            - print text (supports -n flag)\n");
    printf("  cd [directory]         - change directory\n");
    printf("  export VAR=value       - set environment variable\n");
    printf("  unset VAR              - remove environment variable\n");
    printf("  env                    - print all environment variables\n");
    printf("  exit [code]            - exit shell\n");
    printf("\nExternal commands:\n");
    printf("  ls, cat, grep, etc.    - any command in your PATH\n");
    printf("\nTest examples:\n");
    printf("  echo Hello World\n");
    printf("  echo -n No newline\n");
    printf("  pwd\n");
    printf("  cd /tmp\n");
    printf("  export TEST_VAR=hello\n");
    printf("  env | grep TEST\n");
    printf("  unset TEST_VAR\n");
    printf("  ls -la\n");
    printf("\n");
}

int execute_single_command(char *input)
{
    t_data data;
    t_cmd *cmd;
    int result;
    
    // Handle special test commands
    if (!ft_strcmp(input, "help"))
    {
        print_help();
        return 0;
    }
    
    cmd = create_simple_cmd(input);
    if (!cmd)
        return 1;
        
    // Set up data structure
    data.head = cmd;
    
    // Check if it's a builtin that should run in parent process
    if (is_builtin(cmd->full_cmd[0]) && 
        (!ft_strcmp(cmd->full_cmd[0], "cd") || 
         !ft_strcmp(cmd->full_cmd[0], "export") || 
         !ft_strcmp(cmd->full_cmd[0], "unset") ||
         !ft_strcmp(cmd->full_cmd[0], "exit")))
    {
        result = exec_builtin(cmd);
    }
    else
    {
        result = execute_pipeline(&data);
    }
    
    free_cmd(cmd);
    return result;
}

int main(int argc, char **argv, char **envp)
{
    char *input;
    int status = 0;
    
    (void)argc;
    (void)argv;
    
    // Initialize environment
    init_env_list(envp);
    
    // Set up signal handling
    handle_signals();
    
    // Print welcome message
    print_welcome();
    
    // Main shell loop
    while (1)
    {
        // Get input from user
        input = readline("minishell> ");
        
        // Handle EOF (Ctrl+D)
        if (!input)
        {
            printf("\nGoodbye!\n");
            break;
        }
        
        // Skip empty input
        if (!*input)
        {
            free(input);
            continue;
        }
        
        // Add to history
        add_history(input);
        
        // Execute command
        status = execute_single_command(input);
        
        // Print status for testing (optional)
        if (status != 0)
            printf("Command exited with status: %d\n", status);
            
        free(input);
    }
    
    // Cleanup
    free_env_list(g_envp);
    return status;
}
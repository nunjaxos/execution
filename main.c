/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 20:23:16 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/22 13:38:07 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int process_input(char *input, int *last_exit_code)
{
    t_data data = {0};
    t_lexer *lexer = NULL;

    if (!input || !*input)
        return (1);
    
    lexer = init_lexer(input);
    if (!lexer)
        return (0);
    
    data.elem = init_tokens(lexer);
    if (!data.elem)
    {
        free(lexer);
        return (0);
    }
    
    merge_adjacent_word_tokens(&data.elem);
    expand_tokens(data.elem, *last_exit_code);
    
    if (!parse_pipeline(&data))
    {
        cleanup_resources(&data, lexer, NULL);
        return (0);
    }
    
    // Set the exit status pointer in data for signal handlers
    data.exit_status = *last_exit_code;
    
    *last_exit_code = execute_pipeline(&data);
    cleanup_resources(&data, lexer, NULL);
    return (1);
}

int main(int argc, char **argv, char **envp)
{
    char *input;
    int last_exit_code = 0;
    
    (void)argc;
    (void)argv;
    
    // Initialize environment and signal handling
    init_env_list(envp);
    handle_signals(&last_exit_code);
    
    while (1)
    {
        input = readline("minishell$ ");
        if (!input)
        {
            printf("exit\n");
            break;
        }
        if (*input)
        {
            add_history(input);
            process_input(input, &last_exit_code);
        }
        free(input);
    }
    
    free_env_list(g_envp);
    return (last_exit_code);
}

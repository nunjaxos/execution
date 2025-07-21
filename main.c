/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 20:23:16 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/21 09:35:02 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
int	ft_strcmp(const char *s1, const char *s2)
{
	size_t	i;

	i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}


int process_input(char *input, int *last_exit_code)
{
    t_data data;
    t_lexer *lexer;


    init_data(&data, input); // init data
    if (check_empty_line(&data))
        return (1); // Empty line is not an error


    lexer = init_lexer(input); // init lexer
    if (!lexer)
    {
        printf("Error: lexer initialization failed\n");
        return (0);
    }

    // Tokenize and merge the tokens quote 
    data.elem = init_tokens(lexer); // tokenising
    merge_adjacent_word_tokens(&data.elem);

    if (!parse_input(data.elem, input, lexer))
    {
        cleanup_resources(&data, lexer, NULL);
        return (0);
    }
    expand_tokens(data.elem, *last_exit_code); // probably 
    
    if (!parse_pipeline(&data))
    {
        printf("Error: pipeline parsing failed\n");
        printf("DETECTED HERE IN PASE_PIPELINE MIAN.C \n");
        cleanup_resources(&data, lexer, NULL);
        return (0);
    }
    
    // Debug output (remove in production)
    printf("\n--- DEBUG: Parsed Commands ---\n");
    print_pipeline_debug(&data);

    // TODO: Execute commands and update last_exit_code
    // *last_exit_code = execute_pipeline(&data);
    *last_exit_code = 0; // Mock exit status

    // Cleanup
    cleanup_resources(&data, lexer, NULL);
    return (1);
}



int main(int argc, char **argv, char **envp)
{
    char *input;
    t_data data;

    (void)argc;
    (void)argv;
    init_env_list(envp);
    handle_signals();

    while (1)
    {
        input = readline("minishell$ ");
        if (!input)
        {
            printf("exit\n");
            break;
        }
        if (*input)
            add_history(input);

        if (process_input(input, &data.error))
            execute_pipeline(&data);

        free(input);
        cleanup_resources(&data, NULL, NULL);
    }
    return (0);
}


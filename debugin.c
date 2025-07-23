/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debugin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 16:02:54 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/22 13:22:28 by abnemili         ###   ########.fr       */
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

void print_cmd_debug(t_cmd *cmd, int cmd_num)
{
        int i;
        
        printf("=== Command %d ===\n", cmd_num);
        printf("Input FD: %d\n", cmd->in_file);
        printf("Output FD: %d\n", cmd->out_file);
        
        if (cmd->full_cmd)
        {
            printf("Arguments: ");
            i = 0;
            while (cmd->full_cmd[i])
            {
                printf("[%s]", cmd->full_cmd[i]);
                if (cmd->full_cmd[i + 1])
                    printf(", ");
                i++;
            }
            printf("\n");
        }
        else
        {
            printf("Arguments: (none)\n");
        }
        printf("\n");
    }

// Function to print the entire command pipeline
void print_pipeline_debug(t_data *data)
{
    t_cmd *current;
    int cmd_count = 0;
    
    if (!data || !data->head)
    {
        printf("No commands to display (empty pipeline)\n");
        return;
    }
    
    printf("\n==================== PIPELINE DEBUG ====================\n");
    
    current = data->head;
    while (current)
    {
        print_cmd_debug(current, cmd_count + 1);
        current = current->next;
        cmd_count++;
    }
    
    printf("Total commands in pipeline: %d\n", cmd_count);
    printf("========================================================\n\n");
}

// Alternative compact version for quick debugging
void print_pipeline_compact(t_data *data)
{
    t_cmd *current;
    // int cmd_num = 1;
    int i;
    
    if (!data || !data->head)
    {
        printf("Empty pipeline\n");
        return;
    }
    
    printf("Pipeline: ");
    current = data->head;
    while (current)
    {
        printf("[");
        if (current->full_cmd && current->full_cmd[0])
        {
            i = 0;
            while (current->full_cmd[i])
            {
                printf("%s", current->full_cmd[i]);
                if (current->full_cmd[i + 1])
                    printf(" ");
                i++;
            }
        }
        else
        {
            printf("empty");
        }
        
        // Show redirections
        if (current->in_file != STDIN_FILENO)
            printf(" <fd:%d", current->in_file);
        if (current->out_file != STDOUT_FILENO)
            printf(" >fd:%d", current->out_file);
            
        printf("]");
        
        if (current->next)
            printf(" | ");
            
        current = current->next;
        // cmd_num++;
    }
    printf("\n");
}


// Helper function to cleanup resources
void cleanup_resources(t_data *data, t_lexer *lexer, char *input)
{
    if (data->elem)
        free_token_list(data->elem);
    if (data->head)
        free_cmd_list(data->head);
    if (lexer)
        free_lexer(lexer);
    if (input)
        free(input);
}

// Process input and return status (0 = error, 1 = success)

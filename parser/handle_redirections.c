/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handel_redirections.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 11:14:24 by abnemili          #+#    #+#             */
/*   Updated: 2025/06/28 22:53:44 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


int handle_redirection_in(t_data *data, t_elem **current, t_cmd *cmd)
{
    int fd;

    if (!data || !current || !*current || !cmd)
        return (0);

    *current = (*current)->next;
    skip_whitespace_ptr(current);

    if (!*current || (*current)->type != WORD)
        return (0);

    fd = open((*current)->content, O_RDONLY);
    if (fd == -1)
    {
        perror((*current)->content);
        data->file_error = 1;
        *current = (*current)->next;
        return (0);
    }

    if (cmd->in_file != STDIN_FILENO)
        close(cmd->in_file);
    cmd->in_file = fd;

    *current = (*current)->next;
    return (1);
}

// Handles '>' redirection (truncate output)
int handle_redirection_out(t_data *data, t_elem **current, t_cmd *cmd)
{
    int fd;

    if (!data || !current || !*current || !cmd)
        return (0);

    *current = (*current)->next;
    skip_whitespace_ptr(current);

    if (!*current || (*current)->type != WORD)
        return (0);

    fd = open((*current)->content, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror((*current)->content);
        data->file_error = 1;
        *current = (*current)->next;
        return (0);
    }

    if (cmd->out_file != STDOUT_FILENO)
        close(cmd->out_file);
    cmd->out_file = fd;

    *current = (*current)->next;
    return (1);
}

// Handles '>>' redirection (append to output)
int handle_redirection_append(t_data *data, t_elem **current, t_cmd *cmd)
{
    int fd;

    if (!data || !current || !*current || !cmd)
        return (0);

    *current = (*current)->next;
    skip_whitespace_ptr(current);

    if (!*current || (*current)->type != WORD)
        return (0);

    fd = open((*current)->content, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        perror((*current)->content);
        data->file_error = 1;
        *current = (*current)->next;
        return (0);
    }

    if (cmd->out_file != STDOUT_FILENO)
        close(cmd->out_file);
    cmd->out_file = fd;

    *current = (*current)->next;
    return (1);
}

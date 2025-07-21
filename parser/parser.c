/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 11:24:50 by abnemili          #+#    #+#             */
/*   Updated: 2025/06/29 11:37:06 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


int	parse_pipeline(t_data *data)
{
	t_elem	*current;
	t_cmd	*current_cmd;
	t_cmd	*last_cmd;

	if (!data || !data->elem)
		return (0);
	current = data->elem;
	data->head = NULL;
	last_cmd = NULL;
	while (current)
	{
		skip_whitespace_ptr(&current);
		if (!current)
			break;
		if (current->type == PIPE_LINE)
		{
			current = current->next;
			skip_whitespace_ptr(&current);
			if (!current)
				return (0);
			continue;
		}
		current_cmd = parse_command(data, &current);
		if (!current_cmd)
		{
			free_cmd_list(data->head);
			return (0);
		}
		if (!data->head)
			data->head = current_cmd;
		else
			last_cmd->next = current_cmd;
		last_cmd = current_cmd;
	}
	return (data->head != NULL);
}

// Fixed memory cleanup on failure
t_cmd	*parse_command(t_data *data, t_elem **current)
{
	t_cmd	*cmd;

	if (!data || !current)
		return (NULL);
	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->in_file = STDIN_FILENO;
	cmd->out_file = STDOUT_FILENO;
	cmd->full_cmd = NULL;
	cmd->next = NULL;
	if (!parse_arguments(data, current, cmd))
	{
		free_cmd(cmd);
		return (NULL);
	}
	return (cmd);
}

// Fixed argument parsing with better error handling
int	parse_arguments(t_data *data, t_elem **current, t_cmd *cmd)
{
	int	arg_count;
	int	arg_index;

	if (!data || !current || !cmd)
		return (0);
	arg_count = count_command_args(*current);
	if (!allocate_cmd_args(cmd, arg_count))
		return (0);
	arg_index = 0;
	while (*current && (*current)->type != PIPE_LINE)
	{
		skip_whitespace_ptr(current);
		if (!*current || (*current)->type == PIPE_LINE)
			break;
		// Handle both WORD and ENV tokens as arguments
		// 
		else if ((*current)->type == WORD || (*current)->type == ENV)
{
	char *merged = ft_strdup((*current)->content);
	if (!merged)
		return (0);

	t_elem *start = *current;
	t_elem *next = (*current)->next;

	while (next && (next->type == WORD || next->type == ENV))
	{
		// Check if there is whitespace between current and next
		t_elem *tmp = start->next;
		int separated_by_whitespace = 0;
		while (tmp && tmp != next)
		{
			if (tmp->type == WHITE_SPACE)
			{
				separated_by_whitespace = 1;
				break;
			}
			tmp = tmp->next;
		}
		if (separated_by_whitespace)
			break;

		char *tmp_str = ft_strjoin(merged, next->content);
		free(merged);
		if (!tmp_str)
			return (0);
		merged = tmp_str;

		start = next;
		next = next->next;
	}

	if (cmd->full_cmd)
		cmd->full_cmd[arg_index++] = merged;

	*current = start->next;
}

		else if (!process_redirection(data, current, cmd))
			return (0);
	}
	return (1);
}

// Simplified redirection processing
int	process_redirection(t_data *data, t_elem **current, t_cmd *cmd)
{
	if (!data || !current || !*current || !cmd)
		return (0);
	if ((*current)->type == REDIR_IN)
		return (handle_redirection_in(data, current, cmd));
	if ((*current)->type == REDIR_OUT)
		return (handle_redirection_out(data, current, cmd));
	if ((*current)->type == DREDIR_OUT)
		return (handle_redirection_append(data, current, cmd));
	if ((*current)->type == HERE_DOC)
		return (handle_heredoc(data, current, cmd));
	*current = (*current)->next;
	return (1);
}
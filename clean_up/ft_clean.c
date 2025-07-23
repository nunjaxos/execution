/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_clean.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 14:35:39 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/11 15:26:25 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_cmd(t_cmd *cmd)
{
	int	i;

	if (!cmd)
		return ;
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
	if (cmd->in_file != -1 && cmd->in_file != STDIN_FILENO)
		close(cmd->in_file);
	if (cmd->out_file != -1 && cmd->out_file != STDOUT_FILENO)
		close(cmd->out_file);
	free(cmd);
}

void	free_cmd_list(t_cmd *head)
{
	t_cmd	*current;
	t_cmd	*next;

	current = head;
	while (current)
	{
		next = current->next;
		free_cmd(current);
		current = next;
	}
}

void	free_token_list(t_elem *head)
{
	t_elem	*current;
	t_elem	*next;

	current = head;
	while (current)
	{
		next = current->next;
		if (current->content)
			free(current->content);
		free(current);
		current = next;
	}
}

void	free_lexer(t_lexer *lexer)
{
	if (lexer)
	{
		free(lexer);
	}
}

void	cleanup_var_expansion_two(char *name, char *value, int is_special)
{
	if (is_special)
		free(value);
	free(name);
}

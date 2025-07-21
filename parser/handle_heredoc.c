/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 20:04:59 by abnemili          #+#    #+#             */
/*   Updated: 2025/06/13 10:17:37 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	create_heredoc_pipe(int pipe_fd[2])
{
	if (pipe(pipe_fd) == -1)
	{
		perror("minishell: pipe");
		return (0);
	}
	return (1);
}

static void	write_to_pipe(int pipe_fd, const char *line)
{
	write(pipe_fd, line, ft_strlen(line));
	write(pipe_fd, "\n", 1);
}

static int	read_heredoc_lines(const char *delimiter, int pipe_fd)
{
	char	*line;
	int		len;

	while (1)
	{
		write(1, "> ", 2);
		line = get_next_line(STDIN_FILENO);
		if (!line)
		{
			ft_putstr_fd("minishell: warning: here-doc delimited by EOF\n", 2);
			break ;
		}
		len = ft_strlen(line);
		if (len > 0 && line[len - 1] == '\n')
			line[len - 1] = '\0';
		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		write_to_pipe(pipe_fd, line);
		free(line);
	}
	return (1);
}

int	handle_heredoc(t_data *data, t_elem **current, t_cmd *cmd)
{
	int		pipe_fd[2];
	char	*delimiter;

	(void)data;
	*current = (*current)->next;
	skip_whitespace_ptr(current);
	if (!*current || (*current)->type != WORD)
		return (0);
	delimiter = (*current)->content;
	if (!create_heredoc_pipe(pipe_fd))
		return (0);
	if (!read_heredoc_lines(delimiter, pipe_fd[1]))
	{
		close(pipe_fd[1]);
		return (0);
	}
	close(pipe_fd[1]);
	if (cmd->in_file != STDIN_FILENO)
		close(cmd->in_file);
	cmd->in_file = pipe_fd[0];
	*current = (*current)->next;
	return (1);
}

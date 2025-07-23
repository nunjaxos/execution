/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <amouhand@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/22 20:41:11 by amouhand          #+#    #+#             */
/*   Updated: 2024/08/22 01:18:44 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*reading_fd(int fd, char *remainder)
{
	int		n;
	char	*buffer;

	buffer = malloc((BUFFER_SIZE + 1) * sizeof(char));
	if (!buffer)
		return (NULL);
	n = 1;
	while (!ft_strchr_gnl(remainder, '\n') && n != 0)
	{
		n = read(fd, buffer, BUFFER_SIZE);
		if (n == -1)
		{
			free(buffer);
			free(remainder);
			return (NULL);
		}
		buffer[n] = '\0';
		remainder = ft_strjoin_gnl(remainder, buffer);
	}
	free(buffer);
	return (remainder);
}

char	*fill(char *to_cut)
{
	int		i;
	char	*str;

	i = 0;
	if (!to_cut[i])
		return (NULL);
	while (to_cut[i] && to_cut[i] != '\n')
		i++;
	str = malloc(sizeof(char) * (i + 2));
	if (!str)
		return (NULL);
	i = 0;
	while (to_cut[i] && to_cut[i] != '\n')
	{
		str[i] = to_cut[i];
		i++;
	}
	if (to_cut[i] == '\n')
	{
		str[i] = to_cut[i];
		i++;
	}
	str[i] = '\0';
	return (str);
}

char	*update(char *remainder)
{
	char	*str;
	int		i;
	int		j;

	i = 0;
	j = 0;
	while (remainder[i] && remainder[i] != '\n')
		i++;
	if (!remainder[i])
	{
		free(remainder);
		return (NULL);
	}
	str = malloc((sizeof(char)) * (ft_strlen(remainder) - i + 1));
	if (!str)
		return (NULL);
	i++;
	while (remainder[i])
	{
		str[j++] = remainder[i++];
	}
	str[j] = '\0';
	free(remainder);
	return (str);
}

char	*get_next_line(int fd)
{
	static char	*remainder[1024];
	char		*ret;

	if (fd < 0 && BUFFER_SIZE <= 0)
		return (NULL);
	remainder[fd] = reading_fd(fd, remainder[fd]);
	if (!remainder[fd])
		return (NULL);
	ret = fill(remainder[fd]);
	remainder[fd] = update(remainder[fd]);
	return (ret);
}

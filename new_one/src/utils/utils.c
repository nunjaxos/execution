#include "../../include/executor.h"

int	check_for_child(pid_t pid, int exit_stat) 
{
	if (pid == 0)
	{
		get_data()->exit_status = exit_stat;
		free_data(get_data());
		exit(exit_stat);
	}
	else
		get_data()->exit_status = exit_stat;
	return (0);
}

char	*ft_strndup(const char *s, size_t n)
{
	size_t	i;
	char	*dup;

	if (!s)
		return (NULL);
	dup = malloc(n + 1);
	if (!dup)
		return (NULL);
	i = 0;
	while (i < n && s[i])
	{
		dup[i] = s[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

int	ft_strslen(char **strs)
{
	int i = 0;
	if (!strs)
		return (0);
	while (strs[i])
		i++;
	return (i);
}

int	ft_strcmp(char *s1, char *s2)
{
	int	i;

	if (!s1 || !s2)
		return (1);
	i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return (s1[i] - s2[i]);
}

char *ft_strdup_alloc(const char *src)
{
	char	*new;
	int		i = 0;

	while (src[i])
		i++;
	new = ft_malloc(sizeof(char) * (i + 1));
	if (!new)
		return (NULL);
	i = -1;
	while (src[++i])
		new[i] = src[i];
	new[i] = '\0';
	return (new);
}

char *ft_substr_alloc(char const *s, unsigned int start, size_t len)
{
	char	*sub;
	size_t	i = 0;

	if (!s)
		return (NULL);
	if (start >= ft_strlen(s))
		return (ft_strdup_alloc(""));
	if (ft_strlen(s + start) < len)
		len = ft_strlen(s + start);
	sub = ft_malloc(sizeof(char) * (len + 1));
	if (!sub)
		return (NULL);
	while (i < len && s[start + i])
	{
		sub[i] = s[start + i];
		i++;
	}
	sub[i] = '\0';
	return (sub);
}

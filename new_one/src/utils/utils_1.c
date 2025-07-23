#include "../../include/executor.h"

int	str_index(const char *str, int ch)
{
	int	index;

	index = 0;
	while (str[index])
	{
		if (str[index] == ch)
			return (index);
		index++;
	}
	return (-1);
}

char *ft_strjoin_free(char *s1, char *s2, int flag)
{
	char *tmp = ft_strjoin(s1, s2); // returns malloc
	add_alloc(tmp); // <-- track it!
	
	if (s1 && s2 && flag == 2)
	{
		ft_free(s1);
		ft_free(s2);
	}
	else if (s1 && flag == 1)
		ft_free(s1);
	else if (s2 && !flag)
		ft_free(s2);
	return tmp;
}


char	*get_value(char *key, t_env *env)
{
	t_env	*tmp;

	tmp = env;
	while (tmp)
	{
		if (tmp->name && !ft_strcmp(tmp->name, key))
			return (tmp->value);
		tmp = tmp->next;
	}
	return (NULL);
}

int	name_len(char *str)
{
	int	i;

	i = 0;
	while (str[i] && (str[i] != '=' && !(str[i] == '+' && str[i + 1] == '=')))
		i++;
	return (i);
}

char *ft_strjoin_malloc(char *s1, char *s2)
{
	char *result;
	size_t len1 = ft_strlen(s1);
	size_t len2 = ft_strlen(s2);

	result = ft_malloc(len1 + len2 + 1);
	if (!result)
		return NULL;
	ft_memcpy(result, s1, len1);
	ft_memcpy(result + len1, s2, len2);
	result[len1 + len2] = '\0';
	return result;
}

char *ft_strdup_track(const char *s, int len)
{
	char *dup;

	if (len == -1)
		dup = ft_strdup(s);
	else
		dup = ft_strndup(s, len);

	if (!dup)
		return NULL;
	add_alloc(dup);
	return dup;
}

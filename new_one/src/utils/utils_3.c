#include "../../include/executor.h"

t_data	*get_data(void)
{
	static t_data	*data = NULL;

	if (!data)
		data = malloc(sizeof(t_data));
	return (data);
}

int	count_char_array(char **commands)
{
	int	i;

	if (!commands)
		return (0);
	i = 0;
	while (commands[i])
		i++;
	return (i);
}

int	key_len(char *str)
{
	int	i;

	i = 0;
	while (str[i] && (str[i] != '=' && !(str[i] == '+' && str[i + 1] == '=')))
		i++;
	return (i);
}
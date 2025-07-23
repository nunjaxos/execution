#include "../../include/executor.h"

void	ft_env(t_env *env, pid_t pid)
{
	int		i;
	char	**str;

	i = 0;
	str = convert_path_to_array(env);
	while (str[i])
	{
		ft_putendl_fd(str[i], 1);
		i++;
	}
	free_char_array(str);
	check_for_child(pid, 0);
}
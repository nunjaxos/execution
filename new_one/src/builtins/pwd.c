#include "../../include/executor.h"

void	ft_pwd(pid_t pid)
{
	(void)pid;
	char	*pwd;

	pwd = getcwd(NULL, 0);
	add_alloc(pwd);
	ft_putendl_fd(pwd, 1);
	ft_free(pwd);
	check_for_child(pid, 0);
}

// int	main(void)
// {
// 	t_data	data;

// 	// Initialize the t_data structure
// 	ft_bzero(&data, sizeof(t_data));
// 	data.alloc = NULL; // In case your allocator uses this

// 	// Simulate running `pwd` as a built-in (pid = 0)
// 	ft_pwd(0);

// 	return 0;
// }
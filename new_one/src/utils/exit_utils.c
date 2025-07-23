#include "../../include/executor.h"

int	args_check(t_cmd *cmd)
{
	long long	arg;
	size_t		min_limit;
	int			last;

	min_limit = (unsigned long long)(-(LLONG_MIN / 10));
	last = -(LLONG_MIN % 10);
	arg = num(cmd, min_limit, last);
	if (arg == 2)
		return (0);
	return (1);
}

void	check_init(size_t *i, int *j, int *k, int *digit)
{
	*i = 0;
	*j = 0;
	*k = 1;
	*digit = 0;
}

int	overflow_check(size_t test, int digit)
{
	if (test > LLONG_MAX / 10 || (test == LLONG_MAX / 10
			&& (digit > LLONG_MAX % 10)))
		return (2);
	return (0);
}

int	undeflow_check(size_t min_limit, size_t test, int digit, int last)
{
	if (test > min_limit || (test == min_limit && digit > last))
		return (2);
	return (0);
}

int	skip_whitespace(char *command, int i)
{
	while (ft_isspace(command[i]))
		i++;
	return (i);
}

int	ft_isspace(char charac)
{
	if (charac == ' ' || charac == '\t' || charac == '\n'
		|| charac == '\v' || charac == '\f'
		|| charac == '\r')
		return (1);
	return (0);
}

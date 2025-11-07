#include "../minishell.h"

static int	print_all_exports(char **env)
{
	char	**sorted_env;
	int		i;

	sorted_env = sort_env_for_export(env);
	if (!sorted_env)
		return (1);
	i = 0;
	while (sorted_env[i])
	{
		if (print_export_line(sorted_env[i]) == 1)
		{
			free_env_array(sorted_env);
			return (1);
		}
		i++;
	}
	free_env_array(sorted_env);
	return (0);
}

static int	export_single_var(char *arg, char ***env)
{
	char	*name;
	char	*value;
	int		result;

	if (!is_valid_identifier(arg))
	{
		if (write(2, "minishell: export: `", 20) == -1)
			return (1);
		if (write(2, arg, ft_strlen(arg)) == -1)
			return (1);
		if (write(2, "': not a valid identifier\n", 26) == -1)
			return (1);
		return (1);
	}
	parse_assignment(arg, &name, &value);
	if (!name)
		return (1);
	result = set_env_var(env, name, value);
	free(name);
	if (value)
		free(value);
	return (result);
}

static int	handle_export_args(char **args, char ***env)
{
	int	i;
	int	exit_status;
	int	temp_status;

	exit_status = 0;
	i = 1;
	while (args[i])
	{
		temp_status = export_single_var(args[i], env);
		if (temp_status != 0)
			exit_status = temp_status;
		i++;
	}
	return (exit_status);
}

int	builtin_export(char **args, char ***env)
{
	if (!args || !env || !*env)
		return (1);
	if (!args[1])
		return (print_all_exports(*env));
	return (handle_export_args(args, env));
}

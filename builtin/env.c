/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emuzun <emuzun@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 16:02:35 by emuzun            #+#    #+#             */
/*   Updated: 2025/09/02 16:02:36 by emuzun           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	builtin_env(char **env)
{
	int	i;

	if (!env)
		return (1);
	i = 0;
	while (env[i])
	{
		if (write(1, env[i], ft_strlen(env[i])) == -1)
			return (1);
		if (write(1, "\n", 1) == -1)
			return (1);
		i++;
	}
	return (0);
}

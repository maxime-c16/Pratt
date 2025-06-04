/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   singleton.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: macauchy <macauchy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 16:39:31 by macauchy          #+#    #+#             */
/*   Updated: 2025/06/04 16:50:21 by macauchy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Pratt.h"

void	ft_bzero(void *s, size_t n)
{
	size_t	i;
	char	*ptr;

	ptr = (char *)s;
	i = 0;
	while (i < n)
	{
		ptr[i] = 0;
		i++;
	}
}

t_token	*_minishell(void)
{
	static t_token	minishell;
	static bool		init = false;

	if (!init)
	{
		ft_bzero(&minishell, sizeof(t_token));
		init = true;
	}
	return (&minishell);
}

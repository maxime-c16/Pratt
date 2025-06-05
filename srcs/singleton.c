/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   singleton.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: macauchy <macauchy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 16:39:31 by macauchy          #+#    #+#             */
/*   Updated: 2025/06/05 18:19:57 by macauchy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Pratt.h"

void	ft_bzero(void *b, size_t n)
{
	unsigned char	*ptr;

	ptr = (unsigned char *)b;
	while (n)
	{
		*ptr++ = 0;
		n--;
	}
	return ;
}

t_minishell	*_minishell(void)
{
	static t_minishell	minishell;
	static bool			init = false;

	if (!init)
	{
		ft_bzero(&minishell, sizeof(t_minishell));
		init = true;
	}
	return (&minishell);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_expr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: macauchy <macauchy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 15:52:56 by macauchy          #+#    #+#             */
/*   Updated: 2025/06/06 17:53:20 by macauchy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Pratt.h"

t_ast	*parse_expression(int min_bp)
{
	t_token		*token;
	t_ast		*left;
	t_token		*op;
	t_minishell	*minishell;

	minishell = _minishell();
	if (minishell->early_error || minishell->error)
		return (NULL);
	token = advance_token();
	left = parse_prefix(token);
	if (!left || minishell->error || minishell->early_error)
		return (free_ast(left), NULL);
	while (!minishell->error && peek_token()->left_bp > min_bp)
	{
		op = advance_token();
		left = parse_infix(left, op);
		if (!left || minishell->error || minishell->early_error)
		{
			if (op->type == TK_ERROR)
				parser_error_at(op, "Unexpected token", op->text);
			return (free_ast(left), NULL);
		}
	}
	return (left);
}

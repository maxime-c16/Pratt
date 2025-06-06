/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: macauchy <macauchy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 15:55:03 by macauchy          #+#    #+#             */
/*   Updated: 2025/06/06 17:35:48 by macauchy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Pratt.h"

t_token	*peek_token(void)
{
	t_minishell	*minishell;

	minishell = _minishell();
	return (&minishell->tokens[minishell->pos]);
}

t_token	*advance_token(void)
{
	t_minishell	*minishell;

	minishell = _minishell();
	return (&minishell->tokens[minishell->pos++]);
}

void	parser_error_at(t_token *tok, char *msg, char *tk_text)
{
	if (tok->type == TK_ERROR)
		return ;
	dprintf(2, "Error: %s at token '%s' (type: %d)\n", msg, tk_text, tok->type);
	_minishell()->early_error = true;
	if (tok->type == TK_EOF)
		_minishell()->error = true;
	else
		_minishell()->error = true;
}

static void	free_cmd_args(char **args)
{
	size_t	i = 0;

	if (!args)
		return ;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}

void	free_ast(t_ast *ast)
{
	if (!ast)
		return ;
	if (ast->type == AST_CMD)
	{
		free_cmd_args(ast->ast.cmd.args);
	}
	else if (ast->type == AST_PIPE)
	{
		free_ast(ast->ast.pipe.left);
		free_ast(ast->ast.pipe.right);
	}
	else if (ast->type == AST_REDIR)
	{
		free(ast->ast.redir.target);
		free_ast(ast->ast.redir.child);
	}
	else if (ast->type == AST_SUBSHELL)
	{
		free_ast(ast->ast.subshell.sub);
	}
}

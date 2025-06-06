/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_infix.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: macauchy <macauchy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 16:34:13 by macauchy          #+#    #+#             */
/*   Updated: 2025/06/06 17:48:30 by macauchy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Pratt.h"

static t_ast	*infix_pipe(t_ast *left, t_token *op)
{
	t_ast	*node;

	node = malloc(sizeof(t_ast));
	if (!node)
	{
		dprintf(2, "Error: Memory allocation failed for AST node\n");
		exit(EXIT_FAILURE);
	}
	node->type = AST_PIPE;
	node->ast.pipe.left = left;
	node->ast.pipe.right = parse_expression(op->right_bp);
	if (!node->ast.pipe.right)
	{
		free_ast(node);
		parser_error_at(op, "Expected right side of pipe", op->text);
		return (NULL);
	}
	return (node);
}

static t_ast	*infix_logical(t_ast *left, t_token *op)
{
	t_ast	*node;

	node = malloc(sizeof(t_ast));
	if (!node)
	{
		dprintf(2, "Error: Memory allocation failed for AST node\n");
		exit(EXIT_FAILURE);
	}
	if (op->type == TK_AND)
		node->type = AST_AND;
	else if (op->type == TK_OR)
		node->type = AST_OR;
	node->ast.logical.left = left;
	node->ast.logical.right = parse_expression(op->right_bp);
	if (!node->ast.logical.right || _minishell()->error)
	{
		free_ast(node);
		parser_error_at(op, "Expected right side of logical ope", op->text);
		return (NULL);
	}
	return (node);
}

static t_ast	*infix_sequence(t_ast *left, t_token *op)
{
	t_ast	*node;

	node = malloc(sizeof(t_ast));
	if (!node)
	{
		dprintf(2, "Error: Memory allocation failed for AST node\n");
		exit(EXIT_FAILURE);
	}
	if (op->type == TK_BACKGROUND)
		node->type = AST_BG;
	else if (op->type == TK_SEMICOLON)
		node->type = AST_SEQ;
	node->ast.logical.left = left;
	node->ast.logical.right = parse_expression(op->right_bp);
	if (!node->ast.logical.right || _minishell()->error)
	{
		free_ast(node);
		parser_error_at(op, "Expected right side of sequence", op->text);
		return (NULL);
	}
	return (node);
}

static t_ast	*infix_redirection(t_ast *left, t_token *op)
{
	t_token	*file_tok;
	t_ast	*node;

	file_tok = advance_token();
	if (!file_tok || file_tok->type != TK_WORD)
	{
		parser_error_at(op, "Expected file name after redirection", op->text);
		return (NULL);
	}
	node = malloc(sizeof(t_ast));
	if (!node)
	{
		dprintf(2, "Error: Memory allocation failed for AST node\n");
		exit(EXIT_FAILURE);
	}
	node->type = AST_REDIR;
	node->ast.redir.type = op->type;
	node->ast.redir.target = ft_strndup(file_tok->text,
			ft_strlen(file_tok->text));
	if (!node->ast.redir.target)
	{
		free(node);
		dprintf(2, "Error: Memory allocation failed for redirection target\n");
		exit(EXIT_FAILURE);
	}
	node->ast.redir.child = left;
	return (node);
}

t_ast	*parse_infix(t_ast *left, t_token *op)
{
	if (op->type == TK_PIPE)
		return (infix_pipe(left, op));
	else if (op->type == TK_AND || op->type == TK_OR)
		return (infix_logical(left, op));
	else if (op->type == TK_BACKGROUND || op->type == TK_SEMICOLON)
		return (infix_sequence(left, op));
	if (op->type == TK_REDIR_IN || op->type == TK_REDIR_OUT
		|| op->type == TK_APPEND || op->type == TK_HEREDOC
		|| op->type == TK_DUP_IN || op->type == TK_DUP_OUT)
		return (infix_redirection(left, op));
	parser_error_at(op, "Unexpected token in infix", op->text);
	return (NULL);
}

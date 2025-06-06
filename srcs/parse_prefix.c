/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_prefix.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: macauchy <macauchy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 16:02:36 by macauchy          #+#    #+#             */
/*   Updated: 2025/06/06 17:36:52 by macauchy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Pratt.h"

static char	**alloc_args(size_t cap)
{
	char	**args = malloc(sizeof(char *) * cap);
	if (!args)
	{
		dprintf(2, "Error: Memory allocation failed for command args\n");
		exit(EXIT_FAILURE);
	}
	return (args);
}

static void	free_args(char **args, size_t count)
{
	size_t	i;

	for (i = 0; i < count; i++)
		free(args[i]);
	free(args);
}

static char	*dup_arg(const char *text)
{
	char	*arg = ft_strndup(text, ft_strlen(text));
	if (!arg)
	{
		dprintf(2, "Error: Memory allocation failed for command arg\n");
		exit(EXIT_FAILURE);
	}
	return (arg);
}

static char	**grow_args(char **args, size_t old_cnt, size_t *cap)
{
	*cap *= 2;
	args = ft_realloc(args, sizeof(char *) * old_cnt, sizeof(char *) * (*cap));
	if (!args)
	{
		dprintf(2, "Error: Memory allocation failed for command args\n");
		exit(EXIT_FAILURE);
	}
	return args;
}

static t_ast	*prefix_word(t_token *tok)
{
	t_ast	*node;
	size_t	cap = 4;
	size_t	count = 0;
	char	**args;
	t_token	*next_tok;

	node = malloc(sizeof(t_ast));
	if (!node)
	{
		dprintf(2, "Error: Memory allocation failed for AST node\n");
		exit(EXIT_FAILURE);
	}
	node->type = AST_CMD;
	args = alloc_args(cap);
	args[count++] = dup_arg(tok->text);
	while (peek_token()->type == TK_WORD)
	{
		next_tok = advance_token();
		if (count + 1 >= cap)
			args = grow_args(args, count, &cap);
		args[count++] = dup_arg(next_tok->text);
	}
	args[count] = NULL;
	node->ast.cmd.args = args;
	return (node);
}

static t_ast	*prefix_lparen(void)
{
	t_ast	*inner;
	t_ast	*node;

	inner = parse_expression(0);
	if (!inner || _minishell()->early_error || _minishell()->error)
	{
		parser_error_at(peek_token(), "Expected ')'", NULL);
		free_ast(inner);
		return (NULL);
	}
	advance_token();
	node = malloc(sizeof(t_ast));
	node->type = AST_SUBSHELL;
	node->ast.subshell.sub = inner;
	return (node);
}

static t_ast	*prefix_redir(t_token *tok)
{
	t_token	*file_tok;
	t_ast	*cmd;
	t_ast	*node;

	file_tok = advance_token();
	if (!file_tok || file_tok->type != TK_WORD)
	{
		parser_error_at(tok, "Expected file name after redirection", tok->text);
		return (NULL);
	}
	cmd = parse_expression(0);
	if (!cmd)
		return (NULL);
	node = malloc(sizeof(t_ast));
	node->type = AST_REDIR;
	node->ast.redir.type = tok->type;
	node->ast.redir.target = ft_strndup(file_tok->text,
			ft_strlen(file_tok->text));
	if (!node->ast.redir.target)
	{
		dprintf(2, "Error: Memory allocation failed for redirection target\n");
		free_ast(cmd);
		free(node);
		exit(EXIT_FAILURE);
	}
	node->ast.redir.child = cmd;
	return (node);
}

t_ast	*parse_prefix(t_token *tok)
{
	if (tok->type == TK_WORD)
		return (prefix_word(tok));
	if (tok->type == TK_LPAREN)
		return (prefix_lparen());
	if (tok->type == TK_REDIR_IN
		|| tok->type == TK_REDIR_OUT
		|| tok->type == TK_APPEND
		|| tok->type == TK_HEREDOC
		|| tok->type == TK_DUP_IN
		|| tok->type == TK_DUP_OUT)
		return (prefix_redir(tok));
	parser_error_at(tok, "Unexpected token in prefix", tok->text);
	return (NULL);
}

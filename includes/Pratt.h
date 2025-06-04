/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pratt.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: macauchy <macauchy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 16:19:16 by macauchy          #+#    #+#             */
/*   Updated: 2025/06/04 16:39:23 by macauchy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PRATT_H

# define PRATT_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <stdbool.h>
# include <stddef.h>
# include <readline/readline.h>
# include <readline/history.h>

typedef enum e_token_type
{
	TK_WORD,
	TK_PIPE,
	TK_REDIR_IN,
	TK_REDIR_OUT,
	TK_APPEND,
	TK_HEREDOC,
	TK_LPAREN,
	TK_RPAREN,
	TK_EOF,
	TK_ERROR
}				t_token_type;

typedef enum e_ast_type
{
	AST_CMD,
	AST_PIPE,
	AST_REDIR,
	AST_SUBSHELL
}				t_ast_type;

typedef struct s_redir
{
	t_token_type	type;
	char			*target;
	struct s_redir	*next;
}				t_redir;

typedef struct s_token
{
	t_token_type	type;
	char			*text;
	int				left_bp;
	int				right_bp;
}				t_token;

typedef struct s_cmd
{
	char			**args;
	t_redir			*redirs;
	struct s_cmd	*next;
}				t_cmd;

typedef struct s_ast
{
	t_ast_type	type;
	union u_ast
	{
		struct s_ast_cmd
		{
			char		**args;
		} t_ast_cmd;
		struct s_ast_pipe
		{
			struct s_ast	*left;
			struct s_ast	*right;
		} t_ast_pipe;
		struct s_ast_redir
		{
			t_token_type	type;
			char			*target;
			struct s_ast	*child;
		} t_ast_redir;
		struct s_ast_subshell
		{
			struct s_ast	*sub;
		} t_ast_subshell;
	} ast;
}				t_ast;

typedef struct s_minishell
{
	t_token		*tokens;
	unsigned int	pos;
	int		exit_status;
}				t_minishell;

t_minishell	*_minishell(void);

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pratt.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: macauchy <macauchy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 16:19:16 by macauchy          #+#    #+#             */
/*   Updated: 2025/06/06 11:58:05 by macauchy         ###   ########.fr       */
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
	TK_SEMICOLON,
	TK_AND,
	TK_OR,
	TK_DUP_IN,
	TK_DUP_OUT,
	TK_BACKGROUND,
	TK_HERESTRING,
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
	char			*cmdpath;
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
		} cmd;
		struct s_ast_pipe
		{
			struct s_ast	*left;
			struct s_ast	*right;
		} pipe;
		struct s_ast_redir
		{
			t_token_type	type;
			char			*target;
			struct s_ast	*child;
		} redir;
		struct s_ast_subshell
		{
			struct s_ast	*sub;
		} subshell;
	} ast;
}				t_ast;

typedef struct s_minishell
{
	t_token			*tokens;
	char			**cmds;
	unsigned int	pos;
	int				exit_status;
	bool			early_error;
	bool			error;
}				t_minishell;

t_minishell	*_minishell(void);
void	ft_bzero(void *s, size_t n);
int		ft_strlen(const char *s);
int		ft_strcmp(const char *s1, const char *s2);
char	*ft_strndup(char *str, unsigned int len);
void	*ft_realloc(void *ptr, unsigned int old_size,
		unsigned int new_size);
void	append_token(unsigned int *cap, unsigned int *c, char *new_tok);

// debug

void	print_token_array(t_token *tokens);
t_token	*tokenize_to_pratt(char **cmds);

#endif

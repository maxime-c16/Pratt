/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pratt.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: macauchy <macauchy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 16:19:16 by macauchy          #+#    #+#             */
/*   Updated: 2025/06/06 17:46:06 by macauchy         ###   ########.fr       */
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
	AST_SEQ,
	AST_BG,
	AST_AND,
	AST_OR,
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
		struct s_ast_logical
		{
			struct s_ast	*left;
			struct s_ast	*right;
		} logical;
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
	t_ast			*ast;
	size_t			escaped;
}				t_minishell;

t_minishell	*_minishell(void);

void	ft_bzero(void *s, size_t n);
int		ft_strlen(const char *s);
int		ft_isdigit(int c);
int		ft_isspace(char c);
int		ft_isdigit_str(const char *str);
int		ft_strcmp(const char *s1, const char *s2);
void	ft_memcpy(void *dest, const void *src, size_t n);
char	*ft_strndup(char *str, unsigned int len);
void	*ft_realloc(void *ptr, unsigned int old_size,
		unsigned int new_size);
void	append_token(unsigned int *cap, unsigned int *c, char *new_tok);

// debug

void	parse_error_at(const char *word, unsigned int index);

void	print_token_array(t_token *tokens);
void	print_redirection_list(const t_redir *redir);
void	print_ast(const t_ast *node, int indent);
void	print_cmd_list(const t_cmd *cmdlist);
void	print_minishell_state(void);


t_token	*tokenize_to_pratt(char **cmds);
bool	handle_token_error(const char *word, unsigned int i);
void	assign_token_type_and_bp(const char *word, t_token *token);
char	*ft_str3join(const char *s1, const char *s2, const char *s3);

// tokenizer_utils

bool	is_fd_redirection(char **cmds, int i);
void	handle_null_cmds(t_token **tokens, unsigned int *count,
		unsigned int *cap);
void	finalize_token_array(t_token **tokens, unsigned int *count,
		unsigned int *cap);
int		process_regular_token(char **cmds, int i, t_token **tokens,
		unsigned int *count, unsigned int *cap);
int		process_fd_redirection(char **cmd, int i, t_token **tokens,
		unsigned int *count, unsigned int *cap);

// tokenizer

void 	add_token(t_token **arr, unsigned int *count,
		unsigned int *cap, t_token newtok);
void	set_token_fields(t_token *token, t_token_type type, char *text,
		int bp[2]);

// parser

t_token	*peek_token(void);
t_token	*advance_token(void);
t_ast	*parse_expression(int min_bp);
t_ast	*parse_prefix(t_token *tok);
t_ast	*parse_infix(t_ast *left, t_token *op);

void	parser_error_at(t_token *tok, char *msg, char *tk_text);
void	free_ast(t_ast *ast);

#endif

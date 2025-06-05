/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: macauchy <macauchy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 14:33:41 by macauchy          #+#    #+#             */
/*   Updated: 2025/06/05 18:06:59 by macauchy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Pratt.h"


static void add_token(t_token **arr, unsigned int *count,
		unsigned int *cap, t_token newtok)
{
	if (*count + 1 >= *cap)
	{
		if (*cap == 0)
			*cap = 8;
		else
			*cap = *cap * 2;
		*arr = ft_realloc(*arr, sizeof(t_token) * (*count),
				sizeof(t_token) * (*cap));
		if (!*arr)
		{
			perror("minishell: realloc");
			exit(EXIT_FAILURE);
		}
	}
	(*arr)[*count] = newtok;
	(*count)++;
}

static void	parse_error_at(const char *word, unsigned int index)
{
	dprintf(2, "Error: Unexpected token '%s' at index %u\n", word, index);
	_minishell()->error = true;
}

static void	set_token_fields(t_token *token, t_token_type type, char *text,
		int bp[2])
{
	token->type = type;
	if (text)
	{
		token->text = ft_strndup(text, ft_strlen(text));
		if (!token->text)
		{
			dprintf(2, "Error: Memory allocation failed for token text\n");
			exit(EXIT_FAILURE);
		}
	}
	else
		token->text = NULL;
	token->left_bp = bp[0];
	token->right_bp = bp[1];
}

static void	assign_token_type_and_bp(const char *word, t_token *token)
{
	if (!ft_strcmp(word, "|"))
		set_token_fields(token, TK_PIPE, (char *)word, (int []){10, 9});
	else if (!ft_strcmp(word, "<"))
		set_token_fields(token, TK_REDIR_IN, (char *)word, (int []){30, 31});
	else if (!ft_strcmp(word, ">"))
		set_token_fields(token, TK_REDIR_OUT, (char *)word, (int []){30, 31});
	else if (!ft_strcmp(word, ">>"))
		set_token_fields(token, TK_APPEND, (char *)word, (int []){30, 31});
	else if (!ft_strcmp(word, "<<"))
		set_token_fields(token, TK_HEREDOC, (char *)word, (int []){30, 31});
	else if (!ft_strcmp(word, "("))
		set_token_fields(token, TK_LPAREN, (char *)word, (int []){40, 0});
	else if (!ft_strcmp(word, ")"))
		set_token_fields(token, TK_RPAREN, (char *)word, (int []){0, 0});
	else
		set_token_fields(token, TK_WORD, (char *)word, (int []){0, 0});
}

static bool	handle_token_error(const char *word, unsigned int i)
{
	if (!word[0])
	{
		_minishell()->early_error = true;
		parse_error_at(word, i);
		return (true);
	}
	return (false);
}

t_token	*tokenize_to_pratt(char **cmds)
{
	unsigned int	i;
	unsigned int	cap;
	unsigned int	count;
	t_token			*tokens;
	const char		*word;

	i = 0;
	cap = 0;
	count = 0;
	_minishell()->early_error = false;
	tokens = NULL;
	if (!cmds)
	{
		t_token null_tok;
		set_token_fields(&null_tok, TK_ERROR, NULL, (int []){0, 0});
		add_token(&tokens, &count, &cap, null_tok);
		return (tokens);
	}
	while (cmds[i] && !_minishell()->error)
	{
		word = cmds[i];
		t_token		new_token;

		if (handle_token_error(word, i))
			break ;
		assign_token_type_and_bp(word, &new_token);
		add_token(&tokens, &count, &cap, new_token);
		i++;
	}
	if (!_minishell()->error && !_minishell()->early_error)
	{
		t_token eof_token;
		set_token_fields(&eof_token, TK_EOF, NULL, (int []){0, 0});
		add_token(&tokens, &count, &cap, eof_token);
	}
	else
	{
		t_token	err_tok;
		t_token	eof_tok;
		set_token_fields(&err_tok, TK_ERROR, NULL, (int []){0, 0});
		add_token(&tokens, &count, &cap, err_tok);
		set_token_fields(&eof_tok, TK_EOF, NULL, (int []){0, 0});
		add_token(&tokens, &count, &cap, eof_tok);
	}
	t_token	null_tok;
	set_token_fields(&null_tok, TK_ERROR, NULL, (int []){0, 0});
	add_token(&tokens, &count, &cap, null_tok);
	print_token_array(tokens);
	return (tokens);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: macauchy <macauchy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 14:33:41 by macauchy          #+#    #+#             */
/*   Updated: 2025/06/06 17:54:50 by macauchy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Pratt.h"

void	add_token(t_token **arr, unsigned int *count,
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

void	parse_error_at(const char *word, unsigned int index)
{
	dprintf(2, "Error: Unexpected token '%s' at index %u\n", word, index);
	_minishell()->error = true;
}

void	set_token_fields(t_token *token, t_token_type type, char *text,
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

static void	assign_fd_token_type(const char *word, t_token *token,
		unsigned int idx)
{
	if (word[idx] == '>' && word[idx + 1] == '>'
		&& !ft_isspace((unsigned char)word[idx + 2]))
		set_token_fields(token, TK_APPEND, (char *)word, (int []){30, 31});
	else if (word[idx] == '<' && word[idx + 1] == '<'
		&& !ft_isspace((unsigned char)word[idx + 2]))
		set_token_fields(token, TK_HEREDOC, (char *)word, (int []){30, 31});
	else if (word[idx] == '>' && word[idx + 1] == '&'
		&& ft_isdigit((unsigned char)word[idx + 2]))
		set_token_fields(token, TK_DUP_OUT, (char *)word, (int []){30, 31});
	else if (word[idx] == '<' && word[idx + 1] == '&'
		&& ft_isdigit((unsigned char)word[idx + 2]))
		set_token_fields(token, TK_DUP_IN, (char *)word, (int []){30, 31});
	else if (word[idx] == '>' && !ft_isspace((unsigned char)word[idx + 1]))
		set_token_fields(token, TK_REDIR_OUT, (char *)word, (int []){30, 31});
	else if (word[idx] == '<' && !ft_isspace((unsigned char)word[idx + 1]))
		set_token_fields(token, TK_REDIR_IN, (char *)word, (int []){30, 31});
	else
		set_token_fields(token, TK_WORD, (char *)word, (int []){0, 0});
}

static void	assign_simple_token_type(const char *word, t_token *token)
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
	else if (!ft_strcmp(word, ";"))
		set_token_fields(token, TK_SEMICOLON, (char *)word, (int []){20, 21});
	else if (!ft_strcmp(word, "&"))
		set_token_fields(token, TK_BACKGROUND, (char *)word, (int []){0, 0});
	else if (!ft_strcmp(word, "&&"))
		set_token_fields(token, TK_AND, (char *)word, (int []){20, 21});
	else if (!ft_strcmp(word, "||"))
		set_token_fields(token, TK_OR, (char *)word, (int []){20, 21});
	else
		set_token_fields(token, TK_WORD, (char *)word, (int []){0, 0});
}

void	assign_token_type_and_bp(const char *word, t_token *token)
{
	unsigned int	idx;

	idx = 0;
	if (ft_isdigit(word[0]))
	{
		while (ft_isdigit(word[idx]))
			idx++;
		assign_fd_token_type(word, token, idx);
		return;
	}
	assign_simple_token_type(word, token);
}

bool	handle_token_error(const char *word, unsigned int i)
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
	size_t			i;
	size_t			step;
	unsigned int	cap;
	unsigned int	count;
	t_token			*tokens;

	i = 0;
	step = 0;
	cap = 0;
	count = 0;
	_minishell()->early_error = false;
	tokens = NULL;
	if (!cmds)
	{
		handle_null_cmds(&tokens, &count, &cap);
		return (tokens);
	}
	while (cmds[i] && !_minishell()->error)
	{
		if (is_fd_redirection(cmds, i))
		{
			step = process_fd_redirection(cmds, i, &tokens, &count, &cap);
			if (step < 0)
				break ;
			i += step;
			continue ;
		}
		step = process_regular_token(cmds, i, &tokens, &count, &cap);
		if (step < 0)
			break ;
		i += step;
	}
	finalize_token_array(&tokens, &count, &cap);
	print_token_array(tokens);
	return (tokens);
}

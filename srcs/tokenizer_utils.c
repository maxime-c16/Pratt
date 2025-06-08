/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: macauchy <macauchy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 14:19:37 by macauchy          #+#    #+#             */
/*   Updated: 2025/06/06 17:48:14 by macauchy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Pratt.h"

bool	is_fd_redirection(char **cmds, int i)
{
	return (ft_isdigit_str(cmds[i])
		&& cmds[i + 1]
		&& (!ft_strcmp(cmds[i + 1], ">&") || !ft_strcmp(cmds[i + 1], "<&"))
		&& cmds[i + 2]
		&& *cmds[i + 2] != '\0');
}

void	handle_null_cmds(t_token **tokens, unsigned int *count,
		unsigned int *cap)
{
	t_token null_tok;

	set_token_fields(&null_tok, TK_ERROR, NULL, (int []){0, 0});
	add_token(tokens, count, cap, null_tok);
}

void	finalize_token_array(t_token **tokens, unsigned int *count,
		unsigned int *cap)
{
	t_token	tok;
	
	if (!_minishell()->error && !_minishell()->early_error)
	{
		set_token_fields(&tok, TK_EOF, NULL, (int []){0, 0});
		add_token(tokens, count, cap, tok);
	}
	else
	{
		set_token_fields(&tok, TK_ERROR, NULL, (int []){0, 0});
		add_token(tokens, count, cap, tok);
		set_token_fields(&tok, TK_EOF, NULL, (int []){0, 0});
		add_token(tokens, count, cap, tok);
	}
	set_token_fields(&tok, TK_ERROR, NULL, (int []){0, 0});
	add_token(tokens, count, cap, tok);
}

int	process_regular_token(char **cmds, int i, t_token **tokens,
			unsigned int *count, unsigned int *cap)
{
	t_token	new_token;
	char	*word;

	word = cmds[i];
	if (handle_token_error(word, i))
		return (-1);
	assign_token_type_and_bp(word, &new_token);
	add_token(tokens, count, cap, new_token);
	return (1);
}

int	process_fd_redirection(char **cmd, int i, t_token **tokens,
			unsigned int *count, unsigned int *cap)
{
	t_token	new_token;
	char	*joined;

	if (!cmd[i] || !cmd[i + 1])
	{
		_minishell()->early_error = true;
		parse_error_at(cmd[i], i);
		return (-1);
	}
	joined = ft_str3join(cmd[i], cmd[i + 1], cmd[i + 2]);
	if (!joined)
	{
		_minishell()->early_error = true;
		parse_error_at(cmd[i], i);
		return (-1);
	}
	if (handle_token_error(joined, i))
	{
		free(joined);
		return (-1);
	}
	assign_token_type_and_bp(joined, &new_token);
	free(joined);
	add_token(tokens, count, cap, new_token);
	return (3);
}

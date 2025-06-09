/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pratt.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: macauchy <macauchy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 13:27:47 by macauchy          #+#    #+#             */
/*   Updated: 2025/06/06 17:54:06 by macauchy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Pratt.h"

void	free_tokens(char **tokens)
{
	unsigned int	i;

	i = 0;
	if (!tokens)
		return ;
	while (tokens[i])
	{
		free(tokens[i]);
		i++;
	}
	free(tokens);
}

int	ft_isspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || \
			c == '\v' || c == '\f');
}

void	append_token(unsigned int *cap, unsigned int *c, char *new_tok)
{
	t_minishell	*m;

	m = _minishell();
	if (*c + 1 >= *cap)
	{
		if (*cap == 0)
			*cap = 8;
		else
			*cap *= 2;
		m->cmds = ft_realloc(m->cmds, (sizeof(char *) * (*c)), \
			sizeof(char *) * (*cap));
		if (!m->cmds)
		{
			dprintf(2, "Error: Memory allocation failed\n");
			exit(EXIT_FAILURE);
		}
	}
	m->cmds[*c] = new_tok;
	(*c)++;
}

static unsigned int	find_closing_quote(const char *str, char quote)
{
	unsigned int	i;

	i = 1;
	while (str[i] && str[i] != quote)
	{
		if (str[i] == '\\' && quote == '"' && str[i + 1] && str[i + 1] == '"')
			i += 2;
		else
			i++;
	}
	return (i);
}

static unsigned int	copy_quoted_content(const char *str, char quote, \
		unsigned int end, char *buff)
{
	unsigned int	j;
	unsigned int	p;

	j = 1;
	p = 0;
	while (j < end)
	{
		if (str[j] == '\\' && quote == '"' && str[j + 1] && strchr("\"\\$`", str[j + 1]))
		{
			_minishell()->escaped++;
			j++;
			buff[p++] = str[j++];
		}
		else
			buff[p++] = str[j++];
	}
	buff[p] = '\0';
	return (p);
}

static char	*collect_quoted(const char *str, char quote, bool *unclosed)
{
	unsigned int	end;
	char			*buff;

	buff = NULL;
	end = find_closing_quote(str, quote);
	if (str[end] != quote)
	{
		*unclosed = true;
		return (NULL);
	}
	buff = malloc(end);
	if (!buff)
	{
		free_tokens(_minishell()->cmds);
		return (NULL);
	}
	copy_quoted_content(str, quote, end, buff);
	return (buff);
}

char	*ft_strndup(char *str, unsigned int len)
{
	char			*dup;
	unsigned int	i;

	i = 0;
	dup = NULL;
	if (!str)
		return (NULL);
	dup = malloc(len + 1);
	if (!dup)
		return (NULL);
	while (i < len && str[i])
	{
		dup[i] = str[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

static char	*extract_operator(const char *line, unsigned int *i)
{
	unsigned int	op_len;
	char			*op_token;

	op_len = 1;
	op_token = NULL;
	if ((line[*i] == '>' && line[*i + 1] == '>') \
		|| (line[*i] == '<' && line[*i + 1] == '<')
		|| (line[*i] == '|' && line[*i + 1] == '|')
		|| (line[*i] == '&' && line[*i + 1] == '&')
		|| (line[*i] == '>' && line[*i + 1] == '&')
		|| (line[*i] == '<' && line[*i + 1] == '&'))
		op_len = 2;
	op_token = malloc(op_len + 1);
	if (!op_token)
		return (NULL);
	ft_memcpy(op_token, line + *i, op_len);
	op_token[op_len] = '\0';
	*i += op_len;
	return (op_token);
}

static char	*extract_word(const char *line, unsigned int *i)
{
	unsigned int	start;
	unsigned int	len;
	char			*token;

	len = 0;
	start = *i;
	token = NULL;
	while (line[*i] && !ft_isspace(line[*i])
		&& line[*i] != '>' && line[*i] != '<'
		&& line[*i] != '|' && line[*i] != '(' && line[*i] != ')'
		&& line[*i] != ';' && line[*i] != '\'' && line[*i] != '"')
		(*i)++;
	len = *i - start;
	if (len == 0)
		return (NULL);
	token = malloc(len + 1);
	if (!token)
		return (NULL);
	ft_memcpy(token, line + start, len);
	token[len] = '\0';
	return (token);
}

static bool	handle_quote(unsigned int *cap, unsigned int *count,
						char *line, unsigned int *i)
{
	bool			unclosed;
	char			quote;
	char			*qstr;
	unsigned int	skip_len;

	unclosed = false;
	quote = line[*i];
	qstr = collect_quoted(line + *i, quote, &unclosed);
	if (unclosed || !qstr)
	{
		dprintf(2, "Error: Unclosed quote '%c'\n", quote);
		_minishell()->early_error = true;
		return (false);
	}
	skip_len = ft_strlen(qstr) + 2;
	append_token(cap, count, qstr);
	*i += skip_len + _minishell()->escaped;
	return (true);
}

static bool	handle_operator(unsigned int *cap, unsigned int *count,
							char *line, unsigned int *i)
{
	char	*op_token;

	op_token = extract_operator(line, i);
	if (!op_token)
	{
		dprintf(2, "Error: Memory allocation failed\n");
		free_tokens(_minishell()->cmds);
	}
	append_token(cap, count, op_token);
	return (true);
}

static bool	handle_word(unsigned int *cap, unsigned int *count,
						char *line, unsigned int *i)
{
	char	*token;

	token = extract_word(line, i);
	if (!token)
		return (true);
	append_token(cap, count, token);
	return (true);
}

static void	skip_whitespace(const char *line, unsigned int *i)
{
	while (line[*i] && ft_isspace(line[*i]))
		(*i)++;
}

static bool	process_token(unsigned int *cap, unsigned int *count, char *line,
		unsigned int *i)
{
	if (line[*i] == '\'' || line[*i] == '"')
	{
		if (!handle_quote(cap, count, line, i))
			return (false);
	}
	else
	{
		handle_word(cap, count, line, i);
		if (line[*i] == '>' || line[*i] == '<' || line[*i] == '|'
			|| line[*i] == '(' || line[*i] == ')' || line[*i] == ';'
			|| line[*i] == '&')
		{
			if (!handle_operator(cap, count, line, i))
				return (false);
		}
	}
	return (true);
}

char	**split_on_whitespace(char *line)
{
	unsigned int	cap;
	unsigned int	count;
	unsigned int	i;
	t_minishell		*minishell;

	i = 0;
	cap = 8;
	count = 0;
	minishell = _minishell();
	minishell->cmds = malloc(sizeof(char *) * (cap));
	if (!minishell->cmds)
		return (NULL);
	ft_bzero(minishell->cmds, sizeof(char *) * cap);
	while (line[i])
	{
		skip_whitespace(line, &i);
		if (!line[i])
			break ;
		if (!process_token(&cap, &count, line, &i))
		{
			free(line);
			free_token_array();
			free_tokens(minishell->cmds);
			minishell->cmds = NULL;
			return (NULL);
		}
	}
	free(line);
	minishell->cmds[count] = NULL;
	return (minishell->cmds);
}

void	print_tokens(char **tokens)
{
	unsigned int	i;

	i = 0;
	if (!tokens)
	{
		printf("No tokens to display.\n");
		return ;
	}
	while (tokens[i])
	{
		free(tokens[i]);
		i++;
	}
	free(tokens);
}

void	free_token_array(void)
{
	t_minishell	*ms;
	size_t		i;

	ms = _minishell();
	i = 0;
	if (!ms->tokens)
		return ;
	while (ms->tokens[i].text)
	{
		free(ms->tokens[i].text);
		i++;
	}
	free(ms->tokens);
}

void	free_ms_ctx(void)
{
	t_minishell	*ms;

	ms = _minishell();
	free_token_array();
	ms->error = false;
	ms->early_error = false;
	ms->pos = 0;
	ms->escaped = 0;
}

int	main(void)
{
	char	*line;
	char	**words;

	line = NULL;
	while (42)
	{
		line = readline("Pratt> ");
		if (!line)
		{
			write(2, "Error: Failed to read input\n", 28);
			return (1);
		}
		words = split_on_whitespace(line);
		// print_tokens(words);
		_minishell()->tokens = tokenize_to_pratt(words);
		_minishell()->ast = parse_expression(0);
		print_ast(_minishell()->ast, 0);
		_minishell()->cmd_lst = ast_to_cmd(_minishell()->ast);
		free_ast(_minishell()->ast);
		_minishell()->ast = NULL;
		// print_minishell_state();
		print_cmd_list(_minishell()->cmd_lst);
		if (_minishell()->cmd_lst && strcmp(_minishell()->cmd_lst->args[0], "exit") == 0)
		{
			free_ms_ctx();
			free_tokens(words);
			free_cmdlst(_minishell()->cmd_lst);
			break ;
		}
		free_ms_ctx();
		free_tokens(words);
		free_cmdlst(_minishell()->cmd_lst);
		words = NULL;
	}
	return (0);
}

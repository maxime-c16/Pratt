/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pratt.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: macauchy <macauchy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 13:27:47 by macauchy          #+#    #+#             */
/*   Updated: 2025/06/04 17:40:07 by macauchy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Pratt.h"

int	ft_isspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f');
}

static void	append_token(char **arr, unsigned int *cap, unsigned int *count, char new_tok)
{
	if (*count + 1 >= *cap)
	{
		if (*cap == 0)
			*cap = 8;
		else
			*cap *= 2;
		*arr = realloc(*arr, sizeof(**arr) * (*cap));
		if (!*arr)
		{
			dprintf(2, "Error: Memory allocation failed\n");
			exit(EXIT_FAILURE);
		}
	}
	(*arr)[*count] = new_tok;
	(*count)++;
}

static char	*collect_quoted(const char *str, char quote, bool *unclosed)
{
	unsigned int	i;
	unsigned int	len;
	unsigned int	j;
	unsigned int	p;
	char			*buff;

	// First pass find matching quote
	i = 1;
	j = 1;
	p = 0;
	len = 0;
	buff = NULL;
	while (str[i] && str[i] != quote)
	{
		if (str[i] == '\\' && quote == '"' && !str[i + 1])
			i += 2;
		else
			i++;
	}
	if (str[i] != quote)
	{
		*unclosed = true;
		return (NULL);
	}
	// second pass build the string, stripping the quotes
	buff = malloc(i);
	if (!buff)
		return (NULL);
	while (j < i)
	{
		if (str[j] == '\\' && quote == '"' && str[j + 1])
		{
			j++;
			buff[p++] = str[j++];
		}
		else
			buff[p++] = str[j++];
	}
	buff[p] = '\0';
	return (buff);
}

char	**split_on_whitespace(char *line)
{
	unsigned int	cap;
	unsigned int	count;
	unsigned int	i;
	unsigned int	skip_len;
	char			**arr;
	bool			unclosed;
	char			quote;
	char			*qstr;

	cap = 8;
	count = 0;
	i = 0;
	skip_len = 0;
	arr = malloc(sizeof(char *) * (cap + 1));
	unclosed = false;
	if (!arr)
		return (EXIT_FAILURE);
	while (line[i])
	{
		while (ft_isspace(line[i]))
			i++;
		if (!line[i])
			break ;
		if (line[i] == '\'' || line[i] == '"')
		{
			quote = line[i];
			qstr = collect_quoted(line + i, quote, &unclosed);
			if (unclosed)
			{
				dprintf(2, "Error: Unclosed quote '%c'\n", quote);
				for (unsigned int j = 0; j < count; j++)
					free(arr[j]);
				free(arr);
				return (NULL);
			}
			skip_len = strlen(qstr) + 2;
			append_token(&arr, &cap, &count, qstr);
			i += skip_len;
		}
		else
		{
			unsigned int start = i;
			while (line[i] && !ft_isspace(line[i]) && line[i] != '>' && line[i] != '<' && line[i] != '|' && line[i] != '(' && line[i] != ')')
			{
				i++;
				if (start < i)
				{
					unsigned int len = i - start;
					char *token = malloc(len + 1);
					if (!token)
					{
						dprintf(2, "Error: Memory allocation failed\n");
						for (unsigned int j = 0; j < count; j++)
							free(arr[j]);
						free(arr);
						return (NULL);
					}
					memcpy(token, line + start, len);
					token[len] = '\0';
					append_token(&arr, &cap, &count, token);
				}
			}
		}
	}
	
}

int	main(int ac, char **av)
{
	char	*line;
	char	**words;

	line = NULL;
	if (ac != 2)
	{
		write(2, "Usage: ./Pratt <input>\n", 24);
		return (1);
	}
	line = readline("Pratt> ");
	if (!line)
	{
		write(2, "Error: Failed to read input\n", 28);
		return (1);
	}
	//split on whitespace, preserve quoted substrings
	words = split_on_whitespace(line);
	return (0);
}

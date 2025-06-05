/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: macauchy <macauchy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 16:44:08 by macauchy          #+#    #+#             */
/*   Updated: 2025/06/05 16:44:36 by macauchy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Pratt.h"

void print_token_array(t_token *tokens)
{
	printf("== TOKENS ==\n");
	for (int i = 0; tokens[i].type != TK_EOF && tokens[i].type != TK_ERROR; i++)
	{
		printf("[%2d] type: ", i);
		switch (tokens[i].type)
		{
			case TK_WORD:        printf("WORD        "); break;
			case TK_PIPE:        printf("PIPE        "); break;
			case TK_REDIR_IN:    printf("REDIR_IN    "); break;
			case TK_REDIR_OUT:   printf("REDIR_OUT   "); break;
			case TK_APPEND:      printf("APPEND      "); break;
			case TK_HEREDOC:     printf("HEREDOC     "); break;
			case TK_LPAREN:      printf("LPAREN      "); break;
			case TK_RPAREN:      printf("RPAREN      "); break;
			case TK_EOF:         printf("EOF         "); break;
			case TK_ERROR:       printf("ERROR       "); break;
			default:             printf("UNKNOWN     "); break;
		}
		printf("text: %-16s lbp: %-2d rbp: %-2d\n", tokens[i].text, tokens[i].left_bp, tokens[i].right_bp);
	}
	printf("[EOF]\n\n");
}

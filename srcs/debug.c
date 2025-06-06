/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: macauchy <macauchy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 16:44:08 by macauchy          #+#    #+#             */
/*   Updated: 2025/06/06 17:26:28 by macauchy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Pratt.h"

void	print_token_array(t_token *tokens)
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
			case TK_DUP_IN:      printf("DUP_IN      "); break;
			case TK_DUP_OUT:     printf("DUP_OUT     "); break;
			case TK_BACKGROUND:  printf("BACKGROUND  "); break;
			case TK_HERESTRING:  printf("HERESTRING  "); break;
			case TK_SEMICOLON:   printf("SEMICOLON   "); break;
			case TK_AND:         printf("AND         "); break;
			case TK_OR:          printf("OR          "); break;
			default:             printf("UNKNOWN     "); break;
		}
		printf("text: %-16s lbp: %-2d rbp: %-2d\n", tokens[i].text, tokens[i].left_bp, tokens[i].right_bp);
	}
	printf("[EOF]\n\n");
}

#define INDENT_WIDTH 4

static void	print_indent(int level)
{
	for (int i = 0; i < level * INDENT_WIDTH; i++)
		putchar(' ');
}

/* ─────────────── AST Debugger ─────────────── */

void	print_ast(const t_ast *node, int indent)
{
	if (!node)
	{
		print_indent(indent);
		printf("(null)\n");
		return;
	}

	print_indent(indent);
	switch (node->type)
	{
		case AST_CMD:
			printf("AST_CMD: argv = ");
			for (int i = 0; node->ast.cmd.args && node->ast.cmd.args[i]; i++)
				printf("'%s' ", node->ast.cmd.args[i]);
			printf("\n");
			break;

		case AST_PIPE:
			printf("AST_PIPE\n");
			print_indent(indent); printf("├── left:\n");
			print_ast(node->ast.pipe.left, indent + 1);
			print_indent(indent); printf("└── right:\n");
			print_ast(node->ast.pipe.right, indent + 1);
			break;

		case AST_REDIR:
			printf("AST_REDIR: op = ");
			switch (node->ast.redir.type)
			{
				case TK_REDIR_IN:  printf("<"); break;
				case TK_REDIR_OUT: printf(">"); break;
				case TK_APPEND:    printf(">>"); break;
				case TK_HEREDOC:   printf("<<"); break;
				case TK_HERESTRING:printf("<<<"); break;
				case TK_DUP_IN:    printf(">&"); break;
				case TK_DUP_OUT:   printf("<&"); break;
				case TK_BACKGROUND:printf("&"); break;
				case TK_PIPE:      printf("|"); break;
				case TK_AND:       printf("&&"); break;
				case TK_OR:        printf("||"); break;
				case TK_SEMICOLON: printf(";"); break;
				default:           printf("?"); break;
			}
			printf(", target = '%s'\n", node->ast.redir.target);
			print_indent(indent); printf("└── child:\n");
			print_ast(node->ast.redir.child, indent + 1);
			break;

		case AST_SUBSHELL:
			printf("AST_SUBSHELL\n");
			print_ast(node->ast.subshell.sub, indent + 1);
			break;


		case AST_SEQ:
			printf("AST_SEQ\n");
			print_indent(indent); printf("├── left:\n");
			print_ast(node->ast.logical.left, indent + 1);
			print_indent(indent); printf("└── right:\n");
			print_ast(node->ast.logical.right, indent + 1);
			break;
		
		case AST_BG:
			printf("AST_BG\n");
			print_indent(indent); printf("└── child:\n");
			print_ast(node->ast.logical.left, indent + 1);
			break;

		case AST_AND:
			printf("AST_AND\n");
			print_indent(indent); printf("├── left:\n");
			print_ast(node->ast.logical.left, indent + 1);
			print_indent(indent); printf("└── right:\n");
			print_ast(node->ast.logical.right, indent + 1);
			break;

		case AST_OR:
			printf("AST_OR\n");
			print_indent(indent); printf("├── left:\n");
			print_ast(node->ast.logical.left, indent + 1);
			print_indent(indent); printf("└── right:\n");
			print_ast(node->ast.logical.right, indent + 1);
			break;

		default:
			print_indent(indent);
			printf("UNKNOWN AST NODE\n");
			break;
	}
}

/* ─────────────── CMD List Debugger ─────────────── */

void	print_redirection_list(const t_redir *redir)
{
	const t_redir *r = redir;
	while (r)
	{
		printf("  - redir: ");
		switch (r->type)
		{
			case TK_DUP_IN:    printf("dup_in "); break;
			case TK_DUP_OUT:   printf("dup_out "); break;
			case TK_REDIR_IN:  printf("redir_in "); break;
			case TK_REDIR_OUT: printf("redir_out "); break;
			case TK_APPEND:    printf("append "); break;
			case TK_HEREDOC:   printf("heredoc "); break;
			case TK_HERESTRING:printf("herestring "); break;
			case TK_BACKGROUND:printf("background "); break;
			default:           printf("unknown ");
		}
		printf("target: '%s'\n", r->target);
		r = r->next;
	}
}

void	print_cmd_list(const t_cmd *cmdlist)
{
	int i = 0;
	while (cmdlist)
	{
		printf("== CMD #%d ==\n", i++);
		if (cmdlist->args)
		{
			printf("args: ");
			for (int j = 0; cmdlist->args[j]; j++)
				printf("'%s' ", cmdlist->args[j]);
			printf("\n");
		}
		else
			printf("args: (null)\n");

		if (cmdlist->redirs)
		{
			printf("redirections:\n");
			print_redirection_list(cmdlist->redirs);
		}
		else
			printf("redirections: (none)\n");

		cmdlist = cmdlist->next;
		printf("\n");
	}
}

/* ─────────────── Global State Debugger ─────────────── */

void	print_minishell_state(void)
{
	t_minishell *ms = _minishell();
	printf("=== MINI STATE ===\n");
	printf("pos = %u\n", ms->pos);
	printf("error = %s\n", ms->error ? "true" : "false");
	printf("tokens:\n");
	print_token_array(ms->tokens);
}

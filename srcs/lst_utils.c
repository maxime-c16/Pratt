#include "../includes/Pratt.h"

t_cmd	*new_cmd(void)
{
	t_cmd	*cmd;

	cmd = (t_cmd *)malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->next = NULL;
	cmd->redirs = NULL;
	cmd->cmdpath = NULL;
	return (cmd);
}

void	free_cmdlst(t_cmd *cmd)
{
	t_redir		*r;
	t_redir		*redir_next;
	t_cmd		*next;

	r = NULL;
	redir_next = NULL;
	next = NULL;
	while (cmd)
	{
		next = cmd->next;
		if (cmd->args)
		{
			printf("freeing %s\n", cmd->args[0]);
			free_tokens(cmd->args);
		}
		r = cmd->redirs;
		while (r)
		{
			redir_next = r->next;
			free(r->target);
			free(r);
			r = redir_next;
		}
		free(cmd);
		cmd = next;
	}
}

char	**copy_args(char **src)
{
	size_t	n;
	size_t	i;
	char	**dst;

	n = 0;
	i = 0;
	if (src)
	{
		while (src[n])
			n++;
	}
	dst = (char **)malloc(sizeof(char *) * (n + 1));
	if (!dst)
		return (NULL);
	while (i < n)
	{
		dst[i] = ft_strndup(src[i], ft_strlen(src[i]));
		if (!dst[i])
			return (NULL);
		i++;
	}
	dst[n] = NULL;
	return (dst);
}

t_cmd	*flatten_pipe(t_ast *root)
{
	t_cmd	*l_lst;
	t_cmd	*r_lst;
	t_cmd	*it;

	it = NULL;
	l_lst = ast_to_cmd(root->ast.pipe.left);
	r_lst = ast_to_cmd(root->ast.pipe.right);
	if (!l_lst)
		return (r_lst);
	it = l_lst;
	while (it->next)
		it = it->next;
	it->next = r_lst;
	return (l_lst);
}

t_ast	*gather_redirs(t_ast *node, t_redir **rlist)
{
	t_redir	*r;
	t_ast	*child;

	r = NULL;
	child = NULL;
	if (!node)
		return (NULL);
	if (node->type == AST_REDIR)
	{
		r = (t_redir *)malloc(sizeof(t_redir));
		if (!r)
			return (NULL);
		r->type = node->ast.redir.type;
		r->target = ft_strndup(node->ast.redir.target, ft_strlen(node->ast.redir.target));
		r->next = *rlist;
		*rlist = r;
		child = node->ast.redir.child;
		return (gather_redirs(child, rlist));
	}
	return (node);
}

t_cmd	*flatten_pipe_on_redir(t_ast *leaf, t_redir *redir_list)
{
	t_cmd	*pipe_list;
	t_redir	*last;

	last = NULL;
	pipe_list = flatten_pipe(leaf);
	if (!pipe_list)
		return (NULL);
	if (redir_list)
	{
		last = redir_list;
		while (last->next)
			last = last->next;
		last->next = pipe_list->redirs;
		pipe_list->redirs = redir_list;
	}
	return (pipe_list);
}

t_cmd	*switch_type(t_ast *root)
{
	t_redir	*redir_lst;
	t_ast	*leaf;
	t_cmd	*cmd;

	redir_lst = NULL;
	leaf = gather_redirs(root, &redir_lst);
	cmd = NULL;
	if (!leaf)
		return (NULL);
	if (leaf->type == AST_PIPE)
		return (flatten_pipe_on_redir(leaf, redir_lst));
	cmd = new_cmd();
	cmd->redirs = redir_lst;
	if (leaf->type == AST_CMD)
	{
		cmd->args = copy_args(leaf->ast.cmd.args);
	}
	else if (leaf->type == AST_SUBSHELL)
	{
		cmd->args = (char **)malloc(sizeof(char *) * 2);
		if (!cmd->args)
			return (NULL);
		cmd->args[0] = ft_strndup("()", 2);
		cmd->args[1] = NULL;
	}
	else
	{
		cmd->args = (char **)malloc(sizeof(char *));
		if (!cmd->args)
			return (NULL);
	}
	return (cmd);
}

t_cmd	*ast_to_cmd(t_ast *root)
{
	if (!root)
		return (NULL);
	if (root->type == AST_PIPE)
		return (flatten_pipe(root));
	else if (root->type == AST_CMD || root->type == AST_SUBSHELL
		|| root->type == AST_REDIR)
		return (switch_type(root));
	free_ast(root);
	return (NULL);
}

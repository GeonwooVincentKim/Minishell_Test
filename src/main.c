#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include "minishell.h"

size_t	ft_strlen(const char *str)
{
	size_t	i;

	i = 0;
	while (str[i] != '\0')
		i++;
	return (i);
}

size_t	ft_strlcpy(char *dest, const char *src, size_t dest_size)
{
	size_t	i;

	i = 0;
	if (dest_size)
	{
		while (src[i] != '\0' && i < (dest_size - 1))
		{
			dest[i] = src[i];
			i++;
		}
		dest[i] = '\0';
	}
	return (ft_strlen(src));
}

size_t	ft_strlcat(char *dest, const char *src, size_t dest_size)
{
	size_t	len_dest;
	size_t	len_src;
	size_t	i;
	size_t	n;

	if (!dest_size)
		return (ft_strlen(src));
	len_src = ft_strlen(src);
	len_dest = ft_strlen((const char *)dest);
	if (dest_size <= len_dest)
		return (dest_size + len_src);
	n = len_dest;
	i = 0;
	while (src[i] && n + 1 < dest_size)
	{
		dest[n] = src[i];
		i++;
		n++;
	}
	dest[n] = '\0';
	return (len_dest + len_src);
}

int	last_status;

void	interpret(char *line, int *stat_loc)
{
	t_token	*tok;
	t_node	*node;

	tok = tokenize(line);
	if (at_eof(tok))
		;
	else if (syntax_error)
		*stat_loc = ERROR_TOKENIZE;
	else
	{
		node = parse(tok);
		if (syntax_error)
			*stat_loc = ERROR_PARSE;
		else
		{
			expand(node);
			*stat_loc = exec(node);
		}
		free_node(node);
	}
	free_tok(tok);
}

int	main(void)
{
	char	*line;

	rl_outstream = stderr;
	setup_signal();
	last_status = 0;
	while (1)
	{
		line = readline("minishell$ ");
		if (line == NULL)
			break ;
		if (*line)
			add_history(line);
		interpret(line, &last_status);
		free(line);
	}
	exit(last_status);
}

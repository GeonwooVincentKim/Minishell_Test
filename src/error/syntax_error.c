/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_error.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geonwkim <geonwkim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/05 08:56:31 by susami            #+#    #+#             */
/*   Updated: 2024/08/25 01:28:42 by geonwkim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "ft_printf.h"
#include "minishell.h"

#include <stdio.h>

int	ft_dprintf(int fd, const char *format, ...);

void	tokenize_error(const char *location, char **rest, char *line)
{
	g_ctx.syntax_error = true;
	perror_prefix();
	ft_dprintf(STDERR_FILENO,
		"syntax error near unexpected character `%c' in %s\n", *line, location);
	while (*line)
		line++;
	*rest = line;
}

void	parse_error(const char *location, t_token **rest, t_token *tok)
{
	g_ctx.syntax_error = true;
	perror_prefix();
	ft_dprintf(STDERR_FILENO,
		"syntax error near unexpected token `%s' in %s\n", tok->word, location);
	while (tok && !at_eof(tok))
		tok = tok->next;
	*rest = tok;
}

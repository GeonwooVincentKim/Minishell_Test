/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   word.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: susami <susami@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/05 08:56:56 by susami            #+#    #+#             */
/*   Updated: 2023/01/05 11:26:51 by susami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#include <string.h>

/*
DEFINITIONS
       The following definitions are used throughout the rest of this document.
       blank  A space or tab.
	   word   A sequence of characters considered as a single unit by the
	          shell.  Also known as a token.
	   name   A word consisting only of alphanumeric characters and
			  underscores, and beginning with an alphabetic character or an
			  underscore.  Also referred to as an identifier.
       metacharacter
			  A character that, when unquoted, separates words.  One of the
              following:
              |  & ; ( ) < > space tab
       control operator
			  A token that performs a control function.  It is one of the
			  following symbols:
              || & && ; ;; ( ) | <newline>
*/

bool	is_word(const char *s)
{
	return (*s && !is_metacharacter(*s));
}

bool	consume_single_quote(char **rest, char *line)
{
	if (*line == SINGLE_QUOTE_CHAR)
	{
		// skip quote
		line++;
		while (*line && *line != SINGLE_QUOTE_CHAR)
			line++;
		if (*line == '\0')
			tokenize_error("Unclosed single quote", &line, line);
		// skip quote
		else
			line++;
		*rest = line;
		return (true);
	}
	return (false);
}

bool	consume_double_quote(char **rest, char *line)
{
	if (*line == DOUBLE_QUOTE_CHAR)
	{
		// skip quote
		line++;
		while (*line && *line != DOUBLE_QUOTE_CHAR)
			line++;
		if (*line == '\0')
			tokenize_error("Unclosed double quote", &line, line);
		// skip quote
		else
			line++;
		*rest = line;
		return (true);
	}
	return (false);
}

t_token	*word(char **rest, char *line)
{
	const char	*start = line;
	char		*word;

	while (*line && !is_metacharacter(*line))
	{
		if (consume_single_quote(&line, line))
			;
		else if (consume_double_quote(&line, line))
			;
		else
			line++;
	}
	word = strndup(start, line - start);
	if (word == NULL)
		fatal_error("strndup");
	*rest = line;
	return (new_token(word, TK_WORD));
}

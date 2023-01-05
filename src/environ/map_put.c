/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_put.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: susami <susami@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/05 14:08:57 by susami            #+#    #+#             */
/*   Updated: 2023/01/05 14:09:42 by susami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "minishell.h"

#include <string.h>

static void	set_name_value(const char *string, char **name, char **value);

int	map_put(t_map *map, const char *string, bool allow_empty_value, int attr)
{
	int		result;
	char	*name;
	char	*value;

	set_name_value(string, &name, &value);
	if (value == NULL && !allow_empty_value)
	{
		free(name);
		return (-1);
	}
	result = map_set_attr(map, name, value, attr);
	free(name);
	free(value);
	return (result);
}

static void	set_name_value(const char *string, char **name, char **value)
{
	char	*name_end;

	name_end = strchr(string, '=');
	if (name_end == NULL)
	{
		*name = strdup(string);
		*value = NULL;
		if (*name == NULL)
			fatal_error("strdup");
	}
	else
	{
		*name = strndup(string, name_end - string);
		*value = strdup(name_end + 1);
		if (*name == NULL || *value == NULL)
			fatal_error("strdup");
	}
}

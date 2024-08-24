/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   search_path.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geonwkim <geonwkim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/05 08:56:33 by susami            #+#    #+#             */
/*   Updated: 2024/08/24 20:24:31 by geonwkim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <unistd.h>
#include "minishell.h"

#include <string.h>

static char	*search_path_mode(const char *filename, int mode);

char	*search_path(const char *filename)
{
	char	*path;

	path = search_path_mode(filename, X_OK);
	if (path)
		return (path);
	path = search_path_mode(filename, F_OK);
	return (path);
}

static void	set_path(char *path, size_t pathsize,
	const char *filename, char **envpath)
{
	char	*end;

	bzero(path, pathsize);
	end = strchr(*envpath, ':');
	if (*envpath == end)
		ft_strlcpy(path, ".", pathsize);
	else if (end)
		strncpy(path, *envpath, end - *envpath);
	else
		ft_strlcpy(path, *envpath, pathsize);
	ft_strlcat(path, "/", pathsize);
	ft_strlcat(path, filename, pathsize);
	if (end == NULL)
		*envpath = NULL;
	else
		*envpath = end + 1;
}

static char	*search_path_mode(const char *filename, int mode)
{
	char	path[PATH_MAX];
	char	*envpath;
	char	*dup;

	envpath = xgetenv("PATH");
	while (envpath && *envpath)
	{
		set_path(path, PATH_MAX, filename, &envpath);
		// /bin:/usr/bin
		//     ^
		//     end
		if (access(path, mode) == 0)
		{
			dup = strdup(path);
			if (dup == NULL)
				fatal_error("strdup");
			return (dup);
		}
	}
	return (NULL);
}

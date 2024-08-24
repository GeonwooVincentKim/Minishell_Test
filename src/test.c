/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geonwkim <geonwkim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 17:42:57 by geonwkim          #+#    #+#             */
/*   Updated: 2024/08/24 17:45:01 by geonwkim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include	<minishell.h>

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

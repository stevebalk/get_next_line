/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbalk <sbalk@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/03 15:56:13 by sbalk             #+#    #+#             */
/*   Updated: 2023/06/12 15:03:00 by sbalk            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

void	node_to_string(char *str, t_lst *node)
{
	size_t	i;

	i = node->start;
	while (node != NULL && node->tlen)
	{
		i = node->start;
		while (i && node->tlen)
		{
			str[node->tlen - 1] = node->str[i - 1];
			i--;
			node->tlen--;
		}
		node = node->next;
	}
}

int	is_error(ssize_t b_read, t_lst **stash, int fd, char *buffer)
{
	if (b_read <= 0)
	{
		free(buffer);
		if (b_read == 0)
		{
			if (stash[fd])
				return (2);
		}
		else if (b_read == -1)
		{
			if (stash[fd])
				stash[fd] = free_list(stash[fd]);
		}
		return (1);
	}
	return (0);
}

char	*get_next_line(int fd)
{
	static t_lst	*stash[4096];
	char			*buffer;
	int				is_new_line;
	ssize_t			bytes_read;
	int				error;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	is_new_line = 0;
	if (stash[fd])
		is_new_line = check_new_line(stash[fd]);
	while (!is_new_line)
	{
		buffer = malloc(BUFFER_SIZE * sizeof(char));
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		error = is_error(bytes_read, stash, fd, buffer);
		if (error == 2)
			break ;
		else if (error)
			return (NULL);
		stash[fd] = create_node(buffer, stash, fd, bytes_read);
		is_new_line = check_new_line(stash[fd]);
	}
	return (create_new_line(stash[fd], stash, fd));
}

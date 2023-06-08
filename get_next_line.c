/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbalk <sbalk@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/03 15:56:13 by sbalk             #+#    #+#             */
/*   Updated: 2023/06/08 17:28:02 by sbalk            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include "get_next_line.h"

// void	debug_print_list(t_lst **stash, int fd)
// {
// 	size_t i;

// 	i = 0;
// 	t_lst *node = stash[fd];
// 	printf("\n------PRINT LIST----------\n");
// 	while (node != NULL)
// 	{
// 		printf("Node %li: %s\n", i, node->str);
// 		node = node->next;
// 		i++;
// 	}
// 	printf("----------------------------\n");
// }


static t_lst	*free_node(t_lst *node)
{
	t_lst *next_node;

	next_node = node->next;
	if (node->str != NULL)
		free(node->str);
	free(node);
	return (next_node);
}

static void	free_list(t_lst *node)
{
	while (node != NULL)
		node = free_node(node);
}

int	check_new_line(t_lst *node)
{
	while (node->start < node->str_len)
	{
		if (node->str[node->start] == '\n')
		{
			node->tlen++;
			node->start++;
			return (1);
		}
		node->tlen++;
		node->start++;
	}
	return(0);
}

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

char	*create_new_line(t_lst *node, t_lst **stash, int fd)
{
	char	*ret;

	ret = malloc((node->tlen + 1 )* sizeof(char));
	if (!ret)
		return (NULL);
	ret[node->tlen] = '\0';
	node_to_string(ret, node);
	if (node->start == node->str_len)
	{
		free_list(stash[fd]);
		stash[fd] = NULL;
	}
	else
	{
		node->tlen = 0;
		free_list(node->next);
		node->next = NULL;
	}
	return (ret);
}


t_lst	*create_node(char *buffer, t_lst **stash, int fd, ssize_t br)
{
	t_lst	*node;

	node = malloc(sizeof(t_lst));
	if (!node)
		return (NULL);
	node->start = 0;
	node->next = stash[fd];
	node->str = buffer;
	if (node->next)
		node->tlen = node->next->tlen;
	else
		node->tlen = 0;
	node->str_len = br;
	return (node);
}

char	*get_next_line(int fd)
{
	static t_lst	*stash[4096];
	char			*buffer;
	int				is_new_line;
	ssize_t			bytes_read;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	is_new_line = 0;
	if (stash[fd])
		is_new_line = check_new_line(stash[fd]);
	while (!is_new_line)
	{
		buffer = malloc(BUFFER_SIZE * sizeof(char));
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read == 0)
		{
			free(buffer);
			if (stash[fd])
				break;
			else
				return (NULL);
		}
		else if (bytes_read == -1)
		{
			free(buffer);
			if (stash[fd])
			{
				free_list(stash[fd]);
				stash[fd] = NULL;
				return (NULL);
			}
			else
				return (NULL);
		}
		stash[fd] = create_node(buffer, stash, fd, bytes_read);
		is_new_line = check_new_line(stash[fd]);
	}
	return (create_new_line(stash[fd], stash, fd));
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbalk <sbalk@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/03 15:56:26 by sbalk             #+#    #+#             */
/*   Updated: 2023/06/09 12:16:42 by sbalk            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

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

static t_lst	*free_node(t_lst *node)
{
	t_lst	*next_node;

	next_node = node->next;
	if (node->str != NULL)
		free(node->str);
	free(node);
	return (next_node);
}

t_lst	*free_list(t_lst *node)
{
	while (node != NULL)
		node = free_node(node);
	return (NULL);
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
	return (0);
}

char	*create_new_line(t_lst *node, t_lst **stash, int fd)
{
	char	*ret;

	ret = malloc((node->tlen + 1) * sizeof(char));
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

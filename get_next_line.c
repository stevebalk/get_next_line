/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbalk <sbalk@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/03 15:56:13 by sbalk             #+#    #+#             */
/*   Updated: 2023/06/07 11:11:13 by sbalk            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include "get_next_line.h"

void	debug_print_list(t_node **stash, int fd)
{
	size_t i;

	i = 0;
	t_node *node = stash[fd];
	printf("\n\n///// PRINT LIST //////\n");
	while (node != NULL)
	{
		printf("Node %li: %s\n", i, node->str);
		node = node->next;
		i++;
	}
	printf("///////////////////////////\n");
}

static void	is_end_of_line(char *buffer, size_t start, size_t len, t_flags *f)
{
	f->is_new_line = 0;
	f->line_end = start;
	while(f->line_end < len)
	{
		f->line_end++;
		if (buffer[f->line_end] == '\n')
		{
			f->is_new_line = 1;
			break ;
		}
	}
}

static t_node	*free_node(t_node *node)
{
	t_node *next_node;

	next_node = node->next;
	free(node->str);
	free(node);
	return (next_node);
}

static void	free_list(t_node **stash, int fd)
{
	t_node *node;

	node = stash[fd];
	while (node != NULL)
		node = free_node(node);
	stash[fd] = NULL;
}

static t_node	*add_buffer_to_list(t_node **stash, int fd, char *buffer, t_flags *f)
{
	t_node	*new_node;

	new_node = malloc(sizeof(t_node));
	if (new_node == NULL)
	{
		printf("ERROR, List freeed\n");
		free_list(stash, fd);
		return (NULL);
	}
	new_node->str = buffer;
	new_node->next = NULL;
	new_node->tlen = f->line_end;
	if (f->line_end < f->b_read)
		new_node->start = f->line_end;
	if (stash[fd] != NULL)
	{
		// printf("NOT NULL!!!!! with: %s\n", new_node->str);
		new_node->tlen += stash[fd]->tlen;
		new_node->next = stash[fd];
		// printf("NEXT NODE STRING: %s\n", new_node->next->str);
	}
	stash[fd] = new_node;
	// printf("BUFFER: %s\n", stash[fd]->str);
	// printf("LENGTH: %lu\n", stash[fd]->tlen);
	return (stash[fd]);
}


static void	append_to_string(t_node *node, char *str)
{
	size_t	i;

	node = node->next;
	while(node != NULL)
	{
		i = BUFFER_SIZE;
		while (i-- > 0 && node->tlen > 0)
		{
			str[node->tlen - 1] = node->str[i];
			node->tlen -= 1;
		}
		node = free_node(node);
	}
}

static char	*create_return_str(t_node **stash, int fd, t_flags *f)
{
	char	*ret;
	t_node	*new_head_node;
	t_node	*node;
	size_t	new_node_len;

	size_t i = 0;

	new_node_len = f->b_read - f->line_end;
	node = stash[fd];
	new_head_node = node;
	ret = malloc(stash[fd]->tlen + 1);
	if (ret == NULL)
		return (NULL);
	// printf("Length: %lu\n", stash[fd]->tlen);
	while (i < node->tlen - f->line_end) /// DEBUG ////
	{
		ret[i++] = 'A';
	}
	ret[stash[fd]->tlen] = '\0';
	while (f->line_end)
	{
		// printf("Length: %lu\n", stash[fd]->tlen);
		ret[stash[fd]->tlen - 1] = node->str[f->line_end - 1];
		stash[fd]->tlen -= 1;
		f->line_end -= 1;
	}
	// printf("PEW %s\n", node->str);
	append_to_string(node, ret);
	if (new_node_len)
	{
		new_head_node->tlen = new_node_len;
		new_head_node->next = NULL;
	}
	else
		free_node(new_head_node);
	return (ret);
}


char	*get_next_line(int fd)
{
	static t_node	*stash[4096];
	t_flags			flags;
	char			*buffer;
	t_node			*node;

	flags.is_new_line = 0;
	if (stash[fd] != NULL)
	{
		is_end_of_line(stash[fd]->str, stash[fd]->start,
			stash[fd]->str_len, &flags);
	}
	while(!flags.is_new_line)
	{
		buffer = malloc(sizeof(char) * BUFFER_SIZE);
		if (!buffer)
			return (NULL);
		flags.b_read = read(fd, buffer, BUFFER_SIZE);
		if (flags.b_read <= 0)
			return (NULL);
		is_end_of_line(buffer, &flags);
		node = add_buffer_to_list(stash, fd, buffer, &flags);
		if (!node)
			return (NULL);
	}
	// debug_print_list(stash, fd); //// DEBUG /////
	if (stash[fd] != NULL)
		return (create_return_str(stash, fd, &flags));
	return (NULL);
}


// char	*create_return_str(t_node **stash, int fd, char *buf, t_flags *flags)
// {
// 	char	*ret;
// 	size_t	size;
// 	size_t	i;
// 	t_node	*node;

// 	size = flags->line_end;
// 	if (stash[fd])
// 		size = stash[fd]->tlen + flags->line_end + 1;
// 	ret = malloc (size);
// 	if (ret == NULL)
// 		return (NULL);
// 	ret[size--] = '\0';
// 	node = stash[fd];
// 	if (BUFFER_SIZE - flags->line_end != 0)
// 		add_partial_buffer(stash, fd, buf, flags);
// 	while (flags->line_end)
// 		ret[size--] = buf[flags->line_end--];
// 	i = BUFFER_SIZE;
// 	while (size)
// 	{
// 		while (i > 0)
// 			ret[size--] = node->str[i--];
// 		i = BUFFER_SIZE;
// 		node = free_node(node);
// 	}
// 	return (ret);
// }


// void	add_partial_buffer(t_node **stash, int fd, char *buf, t_flags *flags)
// {
// 	t_node	*new_node;

// 	new_node = malloc(sizeof(t_node));
// 	if (new_node == NULL)
// 	{
// 		free_list(stash, fd);
// 		return (NULL);
// 	}
// 	new_node->tlen = BUFFER_SIZE - flags->line_end;
// 	new_node->next = NULL;
// 	new_node->str = buf;
// 	stash[fd] = new_node;
// }

// t_node	*add_full_buffer(t_node **stash, int fd, char *buffer)
// {
// 	t_node	*new_node;

// 	new_node = malloc(sizeof(t_node));
// 	if (new_node == NULL)
// 	{
// 		free_list(stash, fd);
// 		return (NULL);
// 	}
// 	if (stash[fd] != NULL)
// 	{
// 		new_node->tlen = BUFFER_SIZE + stash[fd]->tlen;
// 		new_node->next = stash[fd];
// 		new_node->str = buffer;
// 		stash[fd] = new_node;
// 		return ;
// 	}
// 	new_node->tlen = BUFFER_SIZE;
// 	new_node->next = NULL;
// 	new_node->str = buffer;
// 	stash[fd] = new_node;
// }
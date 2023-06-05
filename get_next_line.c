/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbalk <sbalk@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/03 15:56:13 by sbalk             #+#    #+#             */
/*   Updated: 2023/06/05 18:38:06 by sbalk            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include "get_next_line.h"

void	debug_print_list(t_node **stash, int fd)
{
	t_node *node = stash[fd];
	printf("\n\n///// PRINT LIST //////\n");
	while (node->next != NULL)
	{
		printf("%s\n", node->str);
		node = node->next;
	}
	printf("///////////////////////////\n");
}

static void	is_end_of_line(char *buffer, t_flags *f)
{
	f->is_new_line = 0;
	f->line_end = 0;
	f->new_line_start = 0;
	while(buffer[f->line_end] != '\n' && f->line_end < (size_t)f->b_read)
		f->line_end++;
	if (buffer[f->line_end] == '\n')
	{
		f->is_new_line = 1;
		f->line_end++;
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
	new_node->len = f->line_end;
	if (stash[fd] != NULL)
	{
		new_node->len += stash[fd]->len;
		new_node->next = stash[fd];
	}
	stash[fd] = new_node;
	printf("BUFFER: %s\n", stash[fd]->str);
	printf("%lu\n", stash[fd]->len);
	return (stash[fd]);
}


static void	append_to_string(t_node *node, char *str)
{
	size_t	i;

	while(node->next != NULL || node->len == 0)
	{
		node = node->next;
		i = BUFFER_SIZE;
		while (i-- > 0 && node->len > 0)
		{
			str[node->len - 1] = node->str[i];
			node->len -= 1;
		}
		free_node(node);
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
	ret = malloc(stash[fd]->len + 1);
	if (ret == NULL)
		return (NULL);
	printf("Length: %lu\n", stash[fd]->len);
	while (i < node->len - f->line_end)
	{
		ret[i++] = 'A';
	}
	ret[stash[fd]->len] = '\0';
	while (f->line_end)
	{
		printf("Length: %lu\n", stash[fd]->len);
		ret[stash[fd]->len - 1] = node->str[f->line_end - 1];
		stash[fd]->len -= 1;
		f->line_end -= 1;
	}
	printf("PEW %s\n", node->str);
	append_to_string(node, ret);
	if (new_node_len)
	{
		new_head_node->len = new_node_len;
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
	size_t			i;

	i = 0;
	while (i < 4096)
		stash[i++] = NULL;
	while(!flags.is_new_line)
	{
		buffer = malloc(sizeof(char) * BUFFER_SIZE);
		if (!buffer)
			return (NULL);
		flags.b_read = read(fd, buffer, BUFFER_SIZE);
		if (flags.b_read < 0)
			return (NULL);
		is_end_of_line(buffer, &flags);
		node = add_buffer_to_list(stash, fd, buffer, &flags);
		if (!node)
			return (NULL);
	}
	debug_print_list(stash, fd); //// DEBUG /////
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
// 		size = stash[fd]->len + flags->line_end + 1;
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
// 	new_node->len = BUFFER_SIZE - flags->line_end;
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
// 		new_node->len = BUFFER_SIZE + stash[fd]->len;
// 		new_node->next = stash[fd];
// 		new_node->str = buffer;
// 		stash[fd] = new_node;
// 		return ;
// 	}
// 	new_node->len = BUFFER_SIZE;
// 	new_node->next = NULL;
// 	new_node->str = buffer;
// 	stash[fd] = new_node;
// }
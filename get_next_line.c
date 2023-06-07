/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbalk <sbalk@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/03 15:56:13 by sbalk             #+#    #+#             */
/*   Updated: 2023/06/07 17:30:51 by sbalk            ###   ########.fr       */
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
	printf("\n------PRINT LIST----------\n");
	while (node != NULL)
	{
		printf("Node %li: %s\n", i, node->str);
		node = node->next;
		i++;
	}
	printf("----------------------------\n");
}
// static void	is_end_of_line(char *buffer, size_t start, size_t len, t_flags *f)
static void	is_end_of_line(t_node *node, t_flags *f)
{
	f->is_new_line = 0;
	f->line_end = node->start;
	while(f->line_end < node->str_len)
	{
		f->line_end++;
		node->tlen++;
		if (node->str[f->line_end - 1] == '\n')
		{
			f->is_new_line = 1;
			break ;
		}
	}

	// if (f->line_end < f->b_read)
	// 	new_node->start = f->line_end;

	// new_node->tlen += stash[fd]->tlen; // not NULL
}

static t_node	*free_node(t_node *node)
{
	t_node *next_node;

	next_node = node->next;
	if (node->str != NULL)
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
		printf("ERROR, List freeed\n"); // DEBUG
		free(buffer);
		free_list(stash, fd);
		return (NULL);
	}
	new_node->str = buffer;
	new_node->start = 0;
	new_node->str_len = f->b_read;
	new_node->next = NULL;
	if (stash[fd] != NULL)
	{
		new_node->tlen = stash[fd]->tlen;
		new_node->next = stash[fd];
	}
	else
		new_node->tlen = 0;
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
	size_t	old_start;

	// size_t i = 0;
	// if (stash[fd] == NULL) // Maybe useless
	// 	return (NULL);
	old_start = stash[fd]->start;
	stash[fd]->start = f->line_end;
	// printf("TOTAL_LENGTH: %lu\n", stash[fd]->tlen);
	ret = malloc(stash[fd]->tlen + 1);
	if (ret == NULL)
		return (NULL);
	ret[stash[fd]->tlen] = '\0';
	while (f->line_end > old_start)
	{
		ret[stash[fd]->tlen - 1] = stash[fd]->str[f->line_end - 1];
		stash[fd]->tlen -= 1;
		f->line_end -= 1;
	}
	// printf("PEW %s\n", node->str);
	append_to_string(stash[fd], ret);
	if (stash[fd]->start <= stash[fd]->str_len)
	{
		stash[fd]->tlen = 0;
		stash[fd]->next = NULL;
		// printf("START : %lu\n", stash[fd]->start);
		// printf("LENGTH: %lu\n", stash[fd]->str_len);
	}
	else
	{
		f->file_end = 1;
		free(stash[fd]);
		stash[fd] = NULL;
	}
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
		is_end_of_line(stash[fd], &flags);
	}
	while(!flags.is_new_line)
	{
		buffer = malloc(sizeof(char) * BUFFER_SIZE);
		if (!buffer)
			return (NULL);
		flags.b_read = read(fd, buffer, BUFFER_SIZE);
		if (flags.b_read <= 0 && stash[fd])
		{
			free(buffer);
			// printf("POINTER: %p", stash[fd]);
			// printf("B_READ = %li\n", flags.b_read);
			// stash[fd] = NULL;
			if (flags.b_read == 0 && flags.file_end)
				return (create_return_str(stash, fd, &flags));
			return (NULL);
		}
		// buffer[flags.b_read + 1] = '\0';
		// printf("BUFFER: %s\n", buffer);
		node = add_buffer_to_list(stash, fd, buffer, &flags);
		if (!node)
			return (NULL);
		is_end_of_line(stash[fd], &flags);
	}
	// debug_print_list(stash, fd); //// DEBUG /////
	if (stash[fd] != NULL) // possible pointless //CHECK LATER
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
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbalk <sbalk@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/03 15:56:13 by sbalk             #+#    #+#             */
/*   Updated: 2023/06/07 22:26:52 by sbalk            ###   ########.fr       */
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

static void	is_end_of_line(t_lst *node, t_flags *f)
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
}

static t_lst	*free_node(t_lst *node)
{
	t_lst *next_node;

	next_node = node->next;
	if (node->str != NULL)
		free(node->str);
	free(node);
	return (next_node);
}

static void	free_list(t_lst **stash, int fd)
{
	t_lst *node;

	node = stash[fd];
	while (node != NULL)
		node = free_node(node);
	stash[fd] = NULL;
}

static t_lst	*add_buffer_to_list(t_lst **stash, int fd, char *buffer, t_flags *f)
{
	t_lst	*new_node;

	new_node = malloc(sizeof(t_lst));
	if (new_node == NULL)
	{
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
	return (stash[fd]);
}


static void	append_to_string(t_lst *node, char *str)
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

static char	*create_return_str(t_lst **stash, int fd, t_flags *f)
{
	char	*ret;
	size_t	old_start;

	old_start = stash[fd]->start;
	stash[fd]->start = f->line_end;
	printf("T_LEN = %lu\n", stash[fd]->tlen);
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
	append_to_string(stash[fd], ret);
	if (stash[fd]->start < stash[fd]->str_len)
	{
		stash[fd]->tlen = 0;
		stash[fd]->next = NULL;
	}
	else
	{
		printf("FILE END!!!\n");
		fflush(stdout);
		f->file_end = 1;
		free(stash[fd]->str);
		free(stash[fd]);
		stash[fd] = NULL;
	}
	// printf("%s", ret);
	// fflush(stdout);
	return (ret);
}


char	*get_next_line(int fd)
{
	static t_lst	*stash[4096];
	t_flags			flags;
	char			*buffer;
	t_lst			*node;

	if (BUFFER_SIZE <= 0 || fd < 0)
		return (NULL);
	flags.is_new_line = 0;
	flags.file_end = 0;
	// printf("First Node: %p\n", stash[fd]);
	if (stash[fd] != NULL)
		is_end_of_line(stash[fd], &flags);
	while(!flags.is_new_line)
	{
		buffer = malloc(sizeof(char) * BUFFER_SIZE);
		if (!buffer)
			return (NULL);
		flags.b_read = read(fd, buffer, BUFFER_SIZE);
		if (flags.b_read <= 0)
		{
			// printf("T_LENGTH: %lu\n", stash[fd]->tlen);
			// printf("B_READ: %lu\n", flags.b_read);
			// fflush(stdout);
			free(buffer);
			// if (flags.file_end)
			// 	stash[fd] = NULL;
			// printf("%p\n", stash[fd]);
			if (flags.b_read == 0)
				return (create_return_str(stash, fd, &flags));
			return (NULL);
		}
		node = add_buffer_to_list(stash, fd, buffer, &flags);
		// debug_print_list(stash, fd);
		if (!node)
			return (NULL);
		is_end_of_line(stash[fd], &flags);
	}
	return (create_return_str(stash, fd, &flags));
}


// char	*create_return_str(t_lst **stash, int fd, char *buf, t_flags *flags)
// {
// 	char	*ret;
// 	size_t	size;
// 	size_t	i;
// 	t_lst	*node;

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


// void	add_partial_buffer(t_lst **stash, int fd, char *buf, t_flags *flags)
// {
// 	t_lst	*new_node;

// 	new_node = malloc(sizeof(t_lst));
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

// t_lst	*add_full_buffer(t_lst **stash, int fd, char *buffer)
// {
// 	t_lst	*new_node;

// 	new_node = malloc(sizeof(t_lst));
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
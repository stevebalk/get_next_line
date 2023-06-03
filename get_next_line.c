/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbalk <sbalk@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/03 15:56:13 by sbalk             #+#    #+#             */
/*   Updated: 2023/06/03 17:34:00 by sbalk            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

size_t	is_end_of_line(char *buffer, t_flags *flags)
{
	size_t	i;

	flags->is_new_line = 0;
	flags->line_end = 0;
	flags->new_line_start = 0;

	i = 0;
	while(*buffer != '\n' && i < BUFFER_SIZE)
		i++;
	if (*buffer == '\n')
	{
		i++;
		flags->is_new_line = 1;
		flags->line_end = i;
		if (i < BUFFER_SIZE - 1)
			flags->line_end = i + 1;
	}
	return (i);
}


char	*get_next_line(int fd)
{
	static t_node	stash[4096];
	t_flags			flags;
	char			*buffer;
	ssize_t			bytes_read;

	bytes_read = read(fd, buffer, BUFFER_SIZE);
	while(bytes_read)
	{
		buffer = malloc(sizeof(char) * BUFFER_SIZE);
		if (!buffer)
			return (NULL);
		is_end_of_line(buffer, &flags);
		if (flags.is_new_line)
		{
			return (create_return_str(stash, fd, buffer, &flags));
		}
		else
			add_full_buffer(stash, fd, buffer);
		return (buffer);
	}
	if (bytes_read <= 0)
		return (NULL);
}

void	add_full_buffer(t_node **stash, int fd, char *buffer)
{
	t_node	*new_node;

	new_node = malloc(sizeof(t_node));
	if (new_node == NULL)
	{
		free_list(stash, fd);
		return (NULL);
	}
	if (stash[fd] != NULL)
	{
		new_node->len = BUFFER_SIZE + stash[fd]->len;
		new_node->next = stash[fd];
		new_node->str = buffer;
		stash[fd] = new_node;
		return ;
	}
	new_node->len = BUFFER_SIZE;
	new_node->next = NULL;
	new_node->str = buffer;
	stash[fd] = new_node;
}

void	free_list(t_node **stash, int fd)
{
	t_node *node;
	t_node *next_node;

	node = stash[fd];
	while (node != NULL)
		node = free_node(node);
	stash[fd] = NULL;
}

char	*create_return_str(t_node **stash, int fd, char *buf, t_flags *flags)
{
	char	*ret;
	size_t	size;
	size_t	i;
	size_t	j;
	t_node	*node;

	size = stash[fd]->len + flags->line_end + 1;
	ret = malloc (size);
	if (ret == NULL)
		return (NULL);
	ret[size] = '\0';
	node = stash[fd];
	if (BUFFER_SIZE - flags->line_end)
		add_partial_buffer(stash, fd, buf, flags);
	i = size - 1;
	while (flags->line_end)
		ret[size--] = buf[flags->line_end--];
	j = BUFFER_SIZE;
	while (size)
	{
		while (j > 0)
			ret[size--] = node->str[j--];
		j = BUFFER_SIZE;
		node = free_node(node);
	}
	return (ret);
}

void	add_partial_buffer(t_node **stash, int fd, char *buf, t_flags *flags)
{
	t_node	*new_node;

	new_node = malloc(sizeof(t_node));
	if (new_node == NULL)
	{
		free_list(stash, fd);
		return (NULL);
	}
	new_node->len = BUFFER_SIZE - flags->line_end;
	new_node->next = NULL;
	new_node->str = buf;
	stash[fd] = new_node;
}

t_node	*free_node(t_node *node)
{
	t_node *next_node;

	next_node = node->next;
	free(node->str);
	free(node);
	return (next_node);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbalk <sbalk@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/03 15:56:23 by sbalk             #+#    #+#             */
/*   Updated: 2023/06/09 12:20:30 by sbalk            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_BONUS_H
# define GET_NEXT_LINE_BONUS_H

# include <unistd.h>
# include <stdlib.h>

typedef struct s_lst
{
	char			*str;
	size_t			str_len;
	size_t			start;
	size_t			tlen;
	struct s_lst	*next;
}	t_lst;

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

char	*get_next_line(int fd);
t_lst	*create_node(char *buffer, t_lst **stash, int fd, ssize_t br);
int		check_new_line(t_lst *node);
char	*create_new_line(t_lst *node, t_lst **stash, int fd);
void	node_to_string(char *str, t_lst *node);
t_lst	*free_list(t_lst *node);

#endif
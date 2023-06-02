#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

typedef struct s_node
{
	char	*str;
	void	*next;
} t_node;


#define BUFFER_SIZE 1024

#include <unistd.h>

char	*get_next_line(int fd);

#endif

// 1. read buffer
// 2. check if next line
// 3. if not create new node
//	3.1 malloc size for node
//	3.2 malloc size for buffersize
//	3.3 copy string in allocated var

// 4. add node at the first place in the stash (improves speed a lot)
//	4.1 if stash[xy] not NULL > node->next = stash[xy]adress
//	4.2 stash[xy] = node;
//
//	4.1 if stash == NULL > stash[xy] = node



/*	MERGE FUNCTION
1. stash[fd]
2. get size for whole string + 1 for terminating \0
3. malloc char * with size + 1
4. copy from last to first node
5. free node->str
6. free node
7. set stash[xy] to NULL
7. get rest from buffer after \n and copy everything in a new node
8. stash[xy] = new node (Save rest for next line)
*/



//	clean nodes
//	
//	parser
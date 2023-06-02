#include "get_next_line.h"

size_t	get_end_of_line(char *buffer)
{
	size_t	i;

	i = 0;
	while(*buffer != '\n' && i < BUFFER_SIZE)
		i++;
	if (*buffer == '\n')
		return (i + 1);
	return (0);
}

char	*get_next_line(int fd)
{
	char	*buffer;
	ssize_t	bytes_read;

	if (!buffer)
		buffer = malloc(sizeof(char) * BUFFER_SIZE);
	if (!buffer)
		return NULL;
	while(bytes_read = read(fd, buffer, BUFFER_SIZE))
	{
		return (buffer);
	}
	if (bytes_read <= 0)
		return (NULL);
}
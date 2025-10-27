#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

int ft_popen(const char *file, char *const argv[], char type);

int main(void)
{
	int fd = ft_popen("ls", (char *const[]){"ls", "-a", NULL}, 'r');

	char buf[1];
	while(read(fd, buf, 1))
		write(1, buf, 1);

	close(fd);
	return (0);
}
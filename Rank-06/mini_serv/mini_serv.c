/*
	Subject Includes
*/
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
/*
	Personal Includes
*/
#include <strings.h>
#include <stdbool.h>


/*
	Subject Functions
*/
int extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int	i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char *str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}
/*
	Personal Functions
*/
void	ft_putstr_fd(int fd, char *str) {
	write(fd, str, strlen(str));
	return ;
}

void	ft_fatal_error(int sockfd) {
	ft_putstr_fd(2, "Fatal error\n");
	close(sockfd);
	return ;
}

void	ft_init_socket(int *sockfd, struct sockaddr_in *serveraddr, char *port) {
	// Socket create and verification
	*sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (*sockfd == -1) {
		ft_fatal_error(*sockfd);
		exit(1);
	}

	bzero(serveraddr, sizeof(*serveraddr));

	// Assign IP, PORT
	serveraddr->sin_family = AF_INET;
	serveraddr->sin_addr.s_addr = htonl(2130706433);
	serveraddr->sin_port = htons(atoi(port));

	// Binding newly created socket to given IP and verification
	if ((bind(*sockfd, (const struct sockaddr *)serveraddr, sizeof(*serveraddr))) == -1) {
		ft_fatal_error(*sockfd);
		exit(1);
	}

	// Listening to the newly bound socket and verification
	if (listen(*sockfd, 100) == -1) {
		ft_fatal_error(*sockfd);
		exit(1);
	}

	return ;
}

void	ft_init_fds(int sockfd, fd_set *set, int *max_fd) {
	FD_ZERO(set);
	FD_SET(sockfd, set);
	*max_fd = sockfd;
}

void	ft_init_clients(int	*clients_ids, char **clients_buffers, int *next_id) {
	bzero(clients_ids, sizeof(int) * 65536);
	*next_id = 0;
	bzero(clients_buffers, sizeof(char *) * 65536);
	return ;
}

void	ft_server_broadcast(int mode, int sockfd, int max_fd, int fd, int client_id, fd_set *set) {
	char	server_msg[64];
	if (mode == 1)
		sprintf(server_msg, "server: client %d just arrived\n", client_id);
	else if (mode == 2)
		sprintf(server_msg, "server: client %d just left\n", client_id);

	int	i = 0;
	while (i <= max_fd) {
		if (FD_ISSET(i, set)) {
			if (i != sockfd && i != fd)
				send(i, server_msg, strlen(server_msg), 0);
		}
		i++;
	}
	return ;
}

void	ft_new_client(int sockfd, int *max_fd, int *next_id, fd_set *set, int *clients_ids) {
	struct sockaddr_in	cli;
	socklen_t	len = sizeof(cli);
	int	connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
	if (connfd >= 0) {
		FD_SET(connfd, set);
		if (connfd > *max_fd)
			*max_fd = connfd;

		clients_ids[connfd] = *next_id;
		(*next_id)++;

		ft_server_broadcast(1, sockfd, *max_fd, connfd, clients_ids[connfd], set);
	}
	return ;
}

void	ft_clear_client(int fd, fd_set *set, char **clients_buffers) {
	free(clients_buffers[fd]);
	clients_buffers[fd] = NULL;

	close(fd);
	FD_CLR(fd, set);
	return ;
}

void	ft_client_broadcast(int sockfd, int max_fd, int fd, int client_id, char **msg, fd_set *set) {
	char	*msg_to_send;
	msg_to_send = malloc(strlen(*msg) + 64);
	if (!msg_to_send) {
		ft_fatal_error(sockfd);
		exit(1);
	}

	sprintf(msg_to_send, "client %d: %s", client_id, *msg);

	int	i = 0;
	while (i <= max_fd) {
		if (FD_ISSET(i, set)) {
			if (i != sockfd && i != fd)
				send(i, msg_to_send, strlen(msg_to_send), 0);
		}
		i++;
	}

	free(msg_to_send);
	free(*msg);
}

/*
	Main
*/

int	main(int argc, char **argv) {
	if (argc != 2) {
		ft_putstr_fd(2, "Wrong number of arguments\n");
		exit(1);
	}

	int	sockfd;
	struct sockaddr_in	serveraddr;
	ft_init_socket(&sockfd, &serveraddr, argv[1]);

	int	max_fd;
	fd_set	set;
	ft_init_fds(sockfd, &set, &max_fd);

	int	next_id;
	int	clients_ids[65536];
	char	*clients_buffers[65536];
	ft_init_clients(clients_ids, clients_buffers, &next_id);

	while (true) {
		fd_set	read_set = set;
		if (select(max_fd + 1, &read_set, NULL, NULL, NULL) == -1)
			continue ;
		int	fd = 0;
		while (fd <= max_fd) {
			if (FD_ISSET(fd, &read_set)) {
				if (fd == sockfd)
					ft_new_client(sockfd, &max_fd, &next_id, &set, clients_ids);
				else {
					char	read_buffer[120000];
					bzero(read_buffer, sizeof(read_buffer));
					int	bytes_read = recv(fd, read_buffer, sizeof(read_buffer) - 1, 0);
					if (bytes_read <= 0) {
						ft_server_broadcast(2, sockfd, max_fd, fd, clients_ids[fd], &set);
						ft_clear_client(fd, &set, clients_buffers);
					}
					else {
						clients_buffers[fd] = str_join(clients_buffers[fd], read_buffer);
						if (!clients_buffers[fd]) {
							ft_fatal_error(sockfd);
							exit(1);
						}
						char	*msg;
						int	extract_result;
						extract_result = extract_message(&clients_buffers[fd], &msg);
						while (extract_result == 1) {
							ft_client_broadcast(sockfd, max_fd, fd, clients_ids[fd], &msg, &set);
							extract_result = extract_message(&clients_buffers[fd], &msg);
						}
						if (extract_result == -1) {
							ft_fatal_error(sockfd);
							exit(1);
						}
					}
				}
			}
			fd++;
		}
	}
	return (0);
}
/*
	Subject Includes
*/
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
/*
	Personal Includes
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>


void	fatal(void) {
	write(2, "Fatal error\n", 12);
	exit(1);
}

void	broadcast(char *message, int server, int client, int size) {
	for (int fd = 0; fd <= size; fd++) {
		if (fd != server && fd != client)
			send(fd, message, strlen(message), 0);
	}
}

int	main(int argc, char **argv) {
	int	server;
	int	client;
	int	size;
	int	recv_status;
	int	max_client[65536] = {0};
	int	limit;
	fd_set	old_fd;
	fd_set	new_fd;
	struct sockaddr_in serveraddr;
	char	message[400000];
	char	message_ext[450000];

	if (argc != 2) {
		write(2, "Wrong number of arguments\n", 26);
		exit(1);
	}
	/*
		In Subject
	*/
	server = socket(AF_INET, SOCK_STREAM, 0);
	if (server == -1)
		fatal();
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(2130706433);
	serveraddr.sin_port = htons(atoi(argv[1]));
	if ((bind(server, (const struct sockaddr *)&serveraddr, sizeof(serveraddr))) != 0)
		fatal();
	if (listen(server, 100) != 0)
		fatal();
	/*
		[...]
	*/
	FD_ZERO(&old_fd);
	FD_ZERO(&new_fd);
	FD_SET(server, &new_fd);
	size = server;
	/*
		Loop
	*/
	while (1) {
		old_fd = new_fd;
		if (select(size + 1, &old_fd, NULL, NULL, NULL) == -1)
			fatal();
		for (int client_connected = 0; client_connected <= size; client_connected++) {
			if (FD_ISSET(client_connected, &old_fd) == 0)
				continue ;
			if (client_connected == server) {
				if ((client = accept(server, NULL, NULL)) == -1)
					fatal();
				if (client > size)
					size = client;
				max_client[client] = limit++;
				FD_SET(client, &new_fd);
				bzero(message_ext, 450000);
				sprintf(message_ext, "server: client %d just arrived\n", max_client[client]);
				broadcast(message_ext, server, client, size);
			}
			else {
				bzero(message, 400000);
				bzero(message_ext, 450000);
				recv_status = 1;
				while (recv_status == 1 && (!message[0] || message[strlen(message) - 1] != '\n')) {
					recv_status = recv(client_connected, &message[strlen(message)], 1, 0);
				}
				if (recv_status == 0) {
					sprintf(message_ext, "server: client %d just left\n", max_client[client_connected]);
					broadcast(message_ext, server, client_connected, size);
					FD_CLR(client_connected, &new_fd);
					close(client_connected);
				}
				else {
					sprintf(message_ext, "client %d: %s", max_client[client_connected], message);
					broadcast(message_ext, server, client_connected, size);
				}
			}
		}
	}
	return (0);
}
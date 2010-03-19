#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define DEBUG_MODE 1

#include "log.h"

int main(int argc, char** argv)
{
	unsigned int port;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int server_sock;
	int client_sock;
	unsigned int client_addr_size;
	
	if (argc >= 2) sscanf(argv[1], "%u", &port);
	else port = 80;
	
	server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /* TCP */
	if (server_sock < 0) {
		WARNING("Can not open socket for port %d\n", port);
	}
	
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	
	if (bind(server_sock, (const struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		WARNING("Can not bind to port %d\n", port);
	}
	DEBUG("Binded to port %d\n", port);

	if (listen(server_sock, 0) < 0) {
		WARNING("Can not listen on port %d\n", port);
	}
	DEBUG("Listening on port %d\n", port);
	
	client_addr_size = sizeof(client_addr);
	while(1) {
		client_sock = accept(server_sock, (struct sockaddr *) &client_addr, &client_addr_size);
		if (client_sock < 0) WARNING("Can not accept connection on port %d\n", port);
		DEBUG("Accepted connection on port %d\n", port);
		
	}
	
	shutdown(server_sock, SHUT_RDWR);
	close(server_sock);
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#define DEBUG_MODE 1

#include "config.h"
#include "log.h"
#include "utils.h"
#include "render-index.h"

#define BUFF_SIZE 1024

int server_sock;
int client_sock;

static void response_page(int sock, char * (*content_renderer)())
{
	char *response;
	char headers[]  = "HTTP/1.1 200 OK\n"
	                  "Server: cimon/0.0.1\n"
	                  "Content-Type: text/html; charset=UTF-8\n"
	                  "Connection: close\n"
	                  "\n";
	
	send(sock, headers, strlen(headers), 0);
	DEBUG("%s", headers);
	
	response = content_renderer();
	
	send(sock, response, strlen(response), 0);
	DEBUG("%s", response);
	free(response);
}

static void response_file(int sock, char * file)
{
	FILE *fp;
	char *buff;
	char *filename;
	char length_header[BUFF_SIZE];
	unsigned length;
	
	char headers[]  = "HTTP/1.1 200 OK\n"
	                  "Server: cimon/0.0.1\n"
	                  "Content-Type: image/png\n"
	                  "Accept-Ranges: bytes\n";
	char headers2[] = "Connection: close\n\n";
	
	send(sock, headers, strlen(headers), 0);
	DEBUG("%s", headers);
	
	filename = compose_filename(DATA_DIR, file);
	
	fp = fopen(filename, "r");
	if (fp == NULL) {
		WARNING("Cannot open file %s\n", filename);
		return;
	}
	
	fseek(fp, 0, SEEK_END);
	length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	snprintf(length_header, BUFF_SIZE, "Content-Length: %u\n", length);
	send(sock, length_header, strlen(length_header), 0);
	DEBUG("%s", length_header);
	
	send(sock, headers2, strlen(headers2), 0);
	DEBUG("%s", headers2);
	
	buff = (char *) malloc(length);
	memset(buff, 0, length);
	
	fread(buff, 1, length, fp);
	if (ferror(fp)) {
		WARNING("Cannot read from file %s\n", file);
		fclose(fp);
		return;
	}
	
	send(sock, buff, length, 0);
	free(buff);
	fclose(fp);
}

static void response_404(int sock)
{
	char response[] = "HTTP/1.1 404 Not Found\n"
	                  "Server: cimon/0.0.1\n"
	                  "Content-Type: text/html; charset=UTF-8\n"
	                  "Connection: close\n"
	                  "\n\n";
	send(sock, response, strlen(response), 0);
	DEBUG("%s", response);
}

static void handle_request(int client_sock)
{
	char *buff;
	char request[255];
	char * (*pages[])() = {render_index};
	typedef enum {INDEX} page_types;
	
	buff = (char *) malloc(BUFF_SIZE * sizeof(char));
	memset(buff, 0, BUFF_SIZE);
	
	/* read GET request */
	while (recv(client_sock, buff, BUFF_SIZE, 0) > 0) {
		DEBUG("%s", buff);
		if (strstr(buff, "GET ") != NULL) {
			sscanf(buff, "GET %254s HTTP/1", request);
			DEBUG("Request is: %s\n", request);
		}
		if (strstr(buff, "\n\n") != NULL 
		    || strstr(buff, "\n\r\n") != NULL) break;
		if (buff[0] == '\n' || buff[0] == '\r') break;
		
		memset(buff, 0, BUFF_SIZE);
	}
	
	if (!strcmp(request, "/")) {
		DEBUG("Sending page: %s\n", request);
		response_page(client_sock, pages[INDEX]);
	} else if (!strcmp(request, "/memory.png")) {
		DEBUG("Sending file: %s\n", request);
		response_file(client_sock, "memory.png");
	} else if (!strcmp(request, "/cpu.png")) {
		DEBUG("Sending file: %s\n", request);
		response_file(client_sock, "cpu.png");
	} else if (!strcmp(request, "/net.png")) {
		DEBUG("Sending file: %s\n", request);
		response_file(client_sock, "net.png");
	} else {
		DEBUG("Request unknown: %s\n", request);
		response_404(client_sock);
	}
	
	free(buff);
}

void close_sockets()
{
	DEBUG("%s\n", "Closing sockets");
	
	shutdown(client_sock, SHUT_RDWR);
	close(client_sock);
	
	shutdown(server_sock, SHUT_RDWR);
	close(server_sock);
}

int main(int argc, char** argv)
{
	unsigned int port;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int server_sock;
	int client_sock;
	unsigned int client_addr_size;
	int on = 1;
	
	struct sigaction action;
	action.sa_flags = 0;
	sigemptyset(&action.sa_mask);
	action.sa_handler = close_sockets;
	sigaction(SIGINT,  &action, NULL);
	sigaction(SIGTERM, &action, NULL);
	atexit(close_sockets);
	
	if (argc >= 2) sscanf(argv[1], "%u", &port);
	else port = 80;
	
	server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /* TCP */
	if (server_sock < 0) {
		WARNING("Can not open socket for port %d\n", port);
		return 1;
	}
	
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	
	if (bind(server_sock, (const struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		WARNING("Can not bind to port %d\n", port);
		return 1;
	}
	DEBUG("Binded to port %d\n", port);
	
	if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0) {
		WARNING("Can not setsockopt REUSEADDR to value %d\n", on);
	}

	if (listen(server_sock, 0) < 0) {
		WARNING("Can not listen on port %d\n", port);
		return 1;
	}
	DEBUG("Listening on port %d\n", port);
	
	client_addr_size = sizeof(client_addr);
	while(1) {
		client_sock = accept(server_sock, (struct sockaddr *) &client_addr, &client_addr_size);
		if (client_sock < 0) {
			WARNING("Can not accept connection on port %d\n", port);
			return 1;
		}
		DEBUG("Accepted connection on port %d\n", port);
		
		handle_request(client_sock);
		close(client_sock);
	}
	
	close_sockets();
	return 0;
}

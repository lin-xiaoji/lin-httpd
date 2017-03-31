#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>


int init_socket(u_short port);
void error_die(char msg[]);
int accept_request(int client_sock);




int init_socket(u_short port) {
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1) 
		error_die("socket init fail!");

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		error_die("bind fail");

	if(listen(sock,10) < 0)
		error_die("listen fail");

	return sock;
}



void error_die(char msg[]) {
	printf("%s\n", msg);
	exit(1);
}

void headers(int client, char msg[]) {
	char buf[128];
	strcpy(buf, "HTTP/1.0 ");
	strcat(buf, msg);
	strcat(buf, "\r\n");
	strcat(buf, "Server: lin-httpd\r\n");
	strcat(buf, "Content-Type: text/html\r\n");
	strcat(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
}

void not_found(int client) {
 	headers(client, "404 NOT FOUND");

	char buf[1024];
	sprintf(buf, "404 FILE NOT FOUND\r\n");
	send(client, buf, strlen(buf), 0);
	close(client);
}

void server_file(int client, char filename[]) {
	FILE *fp;
	char filePath[] = "./htdocs";
	strcat(filePath, filename);
	fp = fopen(filePath, "r");
	if (fp == NULL) 
		not_found(client);
	else {
		headers(client, "200 OK");
		char buf[1024];
		fgets(buf, sizeof(buf), fp);
		while (!feof(fp)) {
			send(client, buf, strlen(buf), 0);
			fgets(buf, sizeof(buf), fp);
		}
		fclose(fp);
	}
	
}




int accept_request(int client) {
	//需要先接收完客户端的信息，才能发送，否则浏览器会提示连接重置错误
	char clientHead[1024];
	recv(client, clientHead, sizeof(clientHead), 0);

	char *method;
	char *url;
	method = strtok(clientHead, " ");
	url = strtok( NULL, " ");


	
	if (strcmp(url, "/") == 0) {
		strcpy(url, "/index.html");
	}
	server_file(client, url);

	printf("connected!\n");
	close(client);
}



int main() {
	u_short port = 1234;
	struct sockaddr_in client_name;
	int client_name_len = sizeof(client_name);
	int client_sock = -1;


	int server_sock = init_socket(port);
	printf("httpd running on port: %d\n", port);

	while(1) {
		client_sock = accept(
				server_sock, 
				(struct sockaddr *) &client_name, 
				&client_name_len
			);
		if(client_sock == -1)
			error_die("accept fail");

		accept_request(client_sock);

		close(client_sock);
	}

	close(server_sock);

	return 0;
}
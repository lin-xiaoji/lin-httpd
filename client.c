#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int sockfd;
	int len;
	struct sockaddr_in address;
	int result;
	char s1[] = "Abcdef";
	char s2[] = "123456";

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(1234);
	len = sizeof(address);
	result = connect(sockfd, (struct sockaddr *)&address, len);

	if (result == -1){
		perror("oops: client1");
		exit(1);
	}

	char buf[100] ;
	read(sockfd, buf, sizeof(buf));
	printf("server:%s\n", buf);
	close(sockfd);
	exit(0);
}
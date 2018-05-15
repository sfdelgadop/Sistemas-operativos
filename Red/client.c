#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
/*
struct in_addr{
	unsigned long a_addr
}
struct sockaddr_in{
	short sin_family;
	unsigned short sin_port;
	short in_addr sin_addr;
	char sin_zero[8];
}

NOTA: setsockopt(),
*/
#define MAXLEN 32
#define PORT 3536

int main(){
	int clientfd,r;
	char msg[32];
	socklen_t tama=0;
	struct sockaddr_in client;
	clientfd = socket(AF_INET,SOCK_STREAM,0);
	if(clientfd == -1){
		perror("error en el socket");
		exit(-1);
	}
	client.sin_family = AF_INET;
	client.sin_port = htons(PORT);//htons sirve para evitar en endianismo
	client.sin_addr.s_addr = inet_addr("127.0.0.1");
	bzero(client.sin_zero,8);
	tama = sizeof(struct sockaddr_in);
	r = connect(clientfd,(struct sockaddr*)&client,tama);
	if(r == -1){
		perror("error en el socket");
		exit(-1);
	}
	r = recv(clientfd,msg,MAXLEN,0);
	printf("%s",msg);
	close(clientfd);
}

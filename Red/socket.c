#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <netdb.h>
#include <unistd.h>

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
*/
#define BACKLOG 8
#define MSGSIZE 10

int main(){
	int servfd,clientfd;
	int r;
	int option = 1;

	struct sockaddr_in server,cliente;
	socklen_t tama,tamac;
	servfd = socket(AF_INET,SOCK_STREAM,0);
	setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));//que putas hace buscar
	if(servfd == -1){
		printf("error en el socket");
		return 0;
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(3536);//htons sirve para evitar en endianismo
	server.sin_addr.s_addr = INADDR_ANY;
	bzero(server.sin_zero,8);
	tama = sizeof(struct sockaddr_in);
	r = bind(servfd,(struct sockaddr *)&server,tama);
	if(r == -1){
		perror("error en el socket");
		exit(-1);
	}
	r = listen(servfd,BACKLOG);
	if(r == -1){
		perror("error en el listen");
		exit(-1);
	}
	tamac=0;
	clientfd = accept(servfd,(struct sockaddr *)&cliente,&tamac);
	if(clientfd == -1){
		perror("error en el accept");
		exit(-1);
	}
	
	r=send(clientfd,"hola Mundo",MSGSIZE,0);
	int fff;
	fff = close(clientfd);
	if(fff == -1){
		perror("error en el listen");
	}	
	fff = close(servfd);
	if(fff == -1){
		perror("error en el listen");
	}

}//para atender a varios clientes se necesitan hilos

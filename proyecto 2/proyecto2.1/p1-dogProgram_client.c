#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT 3536
#define NAME 32

struct dogType{//estructura DogType
	char name[32];
	char type[32];
	int age;
	char race[20];
	int height;
	float weight;
	char gender[2];			
};


struct dogType crear_registro_aux(){//en este auxiliar se piden los datos
	char name[32];
	char type[32];
	int age;
	char race[16];
	int height;
	float weight;
	char gender[1];
	struct dogType newPet;
	printf("Bienvenido el menú para ingresar una nueva mascota, a continuación ingrese los datos pedidos\n");
	printf("Ingrese el nombre de la mascota\n");
	scanf("%s",name);
	printf("Ingrese el tipo de la mascota\n");
	scanf("%s",type);
	printf("Ingrese la edad de la mascota\n");
	scanf("%d",&age);
	printf("Ingrese la raza de la mascota\n");
	scanf("%s",race);
	printf("Ingrese la estatura de la mascota\n");
	scanf("%d",&height);
	printf("Ingrese el peso de la mascota\n");
	scanf("%f",&weight);
	printf("Ingrese el género de la mascota\n");
	scanf("%s",gender);
	strcpy(newPet.name,name);//los datos se meten en una estructura
	strcpy(newPet.type,type);
	newPet.age = age;
	strcpy(newPet.race,race);
	newPet.height = height;
	newPet.weight = weight;
	strcpy(newPet.gender,gender);
	return newPet;
}

int main () {

	int clientfd , r , position , regiNum = 0, searchDat, eleccion = 0, delDat = 0, plaSea1 = 0, plaSea2 = 0;
	char msg[32], name[32];
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
//	r = recv(clientfd,msg,MAXLEN,0);
//	printf("%s",msg);

	typedef enum {true,false} bool;
	bool bucle = true;
	while (bucle == true){
		printf("Programa de Administración de Mascotas.\n");//menú principal
		printf("Ingrese 1 para agregar un nuevo registro\n");
		printf("2 para consultar registro\n");
		printf("3 para borrar registro\n");
		printf("4 para buscar un registro\n");
		printf("5 para salir\n");
		int optionInMenuAux = 0;
		int option = 0;
		bool optionInMenu = true;
		scanf("%d",&option);//se escanean las opciones
		r=send(clientfd ,&option ,sizeof(int),0);
		switch(option){
			case 1:
				while (optionInMenu == true){
					struct dogType newPet;
					newPet = crear_registro_aux();//se llama a la función de crear registro				
					r = send( clientfd ,&newPet ,sizeof(struct dogType) ,0);
					r = recv( clientfd ,&position ,sizeof(int) ,0);
					printf("quedo en el posición: %d \n" , position);//da la posición del nuevo registro
					printf("Ingrese 1 si desea volver al menú principal\n");
					printf("Ingrese otro numero si desea ingresar otra mascota\n");
					scanf("%d",&optionInMenuAux);
					r = send( clientfd , &optionInMenuAux , sizeof(int) ,0 );
					if(optionInMenuAux==1){
						optionInMenu = false;
					}
				}
				break;
			case 2:
				while (optionInMenu == true){
					r = recv( clientfd , &regiNum , sizeof(int) ,0 );
					printf ("El numero de registros es: %d \n",regiNum);
					printf("Ingrese el registro que desea buscar\n");
					scanf("%i",&searchDat);
					while(searchDat > regiNum || searchDat < 0){//se verifica la correctitud del dato insertado
						printf("Registro Incorrecto\n");
						printf("por favor ingrese el registro que desea buscar\n");
						scanf("%i",&searchDat);		
					}
					r = send( clientfd , &searchDat , sizeof(int) ,0 );
					printf("¿Desea guardar los cambios realizados en el archivo?\n");
					printf("1 si, 0 no\n");
					scanf("%i",&eleccion);
					r = send( clientfd , &eleccion , sizeof(int) ,0 );
					printf("Ingrese 1 si desea volver al menú principal\n");
					printf("Ingrese otro numero si desea ver otro registro\n");
					scanf("%d",&optionInMenuAux);//se pregunta si desea salir
					r = send( clientfd , &optionInMenuAux , sizeof(int) ,0 );
					if(optionInMenuAux==1){
						optionInMenu = false;
					}
				}
				break;
			case 3:
				while (optionInMenu == true){
					r = recv( clientfd , &regiNum , sizeof(int) ,0 );
					printf ("El numero de registros es: %d \n",regiNum);
					printf("Ingrese el registro que desea buscar\n");
					scanf("%i",&delDat);
					while(delDat > regiNum || delDat < 0){//se verifica la correctitud del dato insertado
						printf("Registro Incorrecto\n");
						printf("por favor ingrese el registro que desea buscar\n");
						scanf("%i",&delDat);		
					}
					r = send( clientfd , &delDat , sizeof(int) ,0 );
					printf("Ingrese 1 si desea volver al menú principal\n");
					printf("Ingrese otro numero si desea ver otro registro\n");
					scanf("%d",&optionInMenuAux);
					r = send( clientfd , &optionInMenuAux , sizeof(int) ,0 );
					if(optionInMenuAux==1){
						optionInMenu = false;
					}
				}
			
				break;
			case 4:
				while (optionInMenu == true){
					printf("Ingrese el nombre que desea buscar\n");//se ingresa el nombre que se desea buscar
					scanf("%s",name);
					r = send( clientfd , name , NAME ,0 );
					r = recv( clientfd , &plaSea1 , sizeof(int) ,0 );
					if(plaSea1 == 0){
						printf("No se encuentra el nombre dentro del archivo \n");
					}else{
						r = recv( clientfd , &plaSea2 , sizeof(int) ,0 );
						if(plaSea1==plaSea2){
							printf("se encuentra en la posición %d \n",plaSea1);
						}else{
							printf("se encuentra desde la posición %d hasta la posición %d \n",plaSea1,plaSea2);
						}
					}
					printf("Ingrese 1 si desea volver al menú principal\n");
					printf("Ingrese otro numero si desea ver otro registro\n");
					scanf("%d",&optionInMenuAux);
					r = send( clientfd , &optionInMenuAux , sizeof(int) ,0 );
					if(optionInMenuAux==1){
						optionInMenu = false;
					}
				}
				break;
			case 5:
				bucle = false;// simplemente lo rompe
				break;
			default:
				printf ("valor incorrecto");// valor por defecto
				break;
		}
	}
}

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
#include <pthread.h>
#include <time.h>
#include <semaphore.h> 
#include <fcntl.h>
#include <sys/stat.h>


#define NAME 32
#define BACKLOG 8
#define NUMTHREADS 32
#define SEMINIT 1
#define CHAR 1


int pipefd [2];
sem_t *semaforo;
pthread_mutex_t mux;

struct dogType{//estructura DogType
	char name[32];
	char type[32];
	int age;
	char race[20];
	int height;
	float weight;
	char gender[2];			
};

int longitud(char *str){//función que decuelve la longitud de una cadena char*
	int j=0;
	while(str[j] != '\0'){j++;}
	return j;
}

char* to_lower(char *str){//devuelbe las cadenas en minusculas
	int i = 0;
	while( str[i] ) {
		str[i]=tolower(str[i]);
		i++;
	}
	return str;
}

unsigned long hash(char *str){//La función de hash que se encarga de asignarle a cada carácter un hash
	to_lower(str);
	unsigned long hash = 0;
	int j=0;
	int min = 0;
	for(int j=0 ; j < longitud(str) ; j++){
		hash = hash*33 + str[j]%33 +j*33 - (long)str[j]*13;
	}
	return hash%1000000;
}

void hashing(int hashList[]){//la funsión que se ancarga de llenar el arreglo con los respectivos hash
	FILE *fp;
	int i = 1;
	fp = fopen("dataDogs.dat","rb+");
	rewind(fp);
	while(!feof(fp)){
		struct dogType pet;
		fread(&pet,sizeof(struct dogType),1,fp);
		if(hashList[hash(pet.name)]==0)
			hashList[hash(pet.name)]=i;//hash por defecto
		i += 1;
	}
	fclose(fp);
}

int lugar(char *str){//función que devuele el lugar en el que se acomoda un nuevo nombre
	FILE *fp;
	fp = fopen("dataDogs.dat","rb+");
	rewind(fp);
	struct dogType newPet;
	int n = 0;
	while( fread(&newPet,sizeof(struct dogType),1,fp)!=0 ){// ciclo que se ejecuta hasta encontrar un nombre mayor al digitado
		n += 1;
//		printf("%s\t%s\n",str,newPet.name);
		if(strcmp(str, newPet.name)<0)
			break;
	}
	fclose(fp);
	return n;
}


int crear_registro(struct dogType pet){//función que crea un nuevo registro clonando el archivo y pegando en la posición correcta el nuevo regitro
	int i=1,position;
	position = lugar(pet.name);
//	printf("%s\n",pet.name);
//	printf("quedo en el posición: %d\n",position);//pide la posición del nuevo registro
	FILE *fp, *temp;
	temp = fopen("temp.dat","wb+");
	fp = fopen("dataDogs.dat","rb+");
	struct dogType newPet;
	while( fread(&newPet,sizeof(struct dogType),1,fp)!=0 && position != i){//copia la primera parte del archivo
		fwrite(&newPet,sizeof(struct dogType),1,temp);
		i += 1;
		if (position == i )
			break;
	}

	fwrite(&pet,sizeof(struct dogType),1,temp);//algrega el nuevo elemento

	while( fread(&newPet,sizeof(struct dogType),1,fp)!=0 )//copia la segunda parte del archivo
		fwrite(&newPet,sizeof(struct dogType),1,temp);
	
	fclose(fp);
	fclose(temp);
	remove("dataDogs.dat");//se elimina el original
	rename("temp.dat","dataDogs.dat");//se renombra el segundario
	return position;
}

void eliminar_registro(int dato_a_eliminar){//se pide el dato a eliminar y se elimina del archivo
	FILE *fp, *temp;
	struct dogType newPet;
	temp = fopen("temp.dat","wb+");
	fp = fopen("dataDogs.dat","rb+");
	int i=1;
	while( fread(&newPet,sizeof(struct dogType),1,fp)!=0 && dato_a_eliminar != i){// se copia la primera parte del archivo sin tener el cuenta el que se desea eliminar
		fwrite(&newPet,sizeof(struct dogType),1,temp);
		i += 1;
	}

	while( fread(&newPet,sizeof(struct dogType),1,fp)!=0 )//se copia la segunda parte del archivo
		fwrite(&newPet,sizeof(struct dogType),1,temp);

	fclose(fp);
	fclose(temp);
	remove("dataDogs.dat");// se elimina el original
	rename("temp.dat","dataDogs.dat");//el temporal toma su lugar
}

int registers_number(){
	FILE *fp;
	int numero_de_registros;
	fp = fopen("dataDogs.dat","rb+");
	fseek(fp,0,SEEK_END);//se envía el puntero al final del archivo
	numero_de_registros = ftell(fp)/sizeof(struct dogType);//se obtiene la cantidad de elementos del archivo
	fclose(fp);
	return numero_de_registros;
}

void changes(int searchDat){
	FILE *fp;
	struct dogType pet;
	char name[32];
	char type[32];
	char trh[32];//sirve para guardar la basura nombre: type:... etc
	int age;
	char race[20];
	int height;
	float weight;
	char gender[1];
	fp = fopen("probe.txt","r+");
	rewind(fp);
	fscanf(fp,"%s %s",trh,name);
	fscanf(fp,"%s %s",trh,type);
	fscanf(fp,"%s %s",trh,&age);
	fscanf(fp,"%s %s",trh,race);
	fscanf(fp,"%s %d",trh,&height);
	fscanf(fp,"%s %f",trh,&weight);
	fscanf(fp,"%s %s",trh,gender);
	strcpy(pet.name,name);
	strcpy(pet.type,type);
	pet.age = age;
	strcpy(pet.race,race);
	pet.height = height;
	pet.weight = weight;
	strcpy(pet.gender,gender);
	fclose(fp);
	eliminar_registro(searchDat);//se elimina el registro antiguo
	crear_registro(pet);//se agrega el registro nuevo
	system("killall gedit");//se cierra el archivo de vizualisación para que no pueda ser modificado más
	remove("probe.txt");
}


void buscar_registro(int searchDat){//función para buscar el registro
	FILE *fp;
	int eleccion;
	fp = fopen("dataDogs.dat","rb+");
	struct dogType newPet;
	fseek(fp,(searchDat-1)*sizeof(struct dogType),SEEK_SET);//se acomoda el puntero en la posicion deseada
	fread(&newPet,sizeof(struct dogType),1,fp);//se sube a una estructura
	fclose(fp);
	fp = fopen("probe.txt","w+");//todo se sube a la pantalla de vizualisación
	fprintf(fp,"Nombre: %s\n",newPet.name);
	fprintf(fp,"Tipo: %s\n",newPet.type);
	fprintf(fp,"Edad: %i\n",newPet.age);
	fprintf(fp,"Raza: %s\n",newPet.race);
	fprintf(fp,"Tamaño: %i\n",newPet.height);
	fprintf(fp,"Peso: %f\n",newPet.weight);
	fprintf(fp,"Genero: %s\n",newPet.gender);	
	fclose(fp);
	system("gedit probe.txt");
}

int search_i(int hashList[], char name[]){//función para buscar en el archivo
	FILE *fp;
	FILE *fp2;
	int aux = 0, i;
	fp = fopen("dataDogs.dat","rb+");
	i = hashList[hash(name)];//se llama a hash
	if(hashList[hash(name)]==0){
		fclose(fp);
		return 0;
	}
	fseek(fp,i*sizeof(struct dogType),SEEK_SET);//se va a la posicion solicitada
	struct dogType newPet;
	fread(&newPet,sizeof(struct dogType),1,fp);
	
	fp2 = fopen("probe2.txt","a+");//todo se sube a la pantalla de vizualisación
	fprintf(fp2,"Nombre: %s\n",newPet.name);
	fprintf(fp2,"Tipo: %s\n",newPet.type);
	fprintf(fp2,"Edad: %i\n",newPet.age);
	fprintf(fp2,"Raza: %s\n",newPet.race);
	fprintf(fp2,"Tamaño: %i\n",newPet.height);
	fprintf(fp2,"Peso: %f\n",newPet.weight);
	fprintf(fp2,"Genero: %s\n\n",newPet.gender);	
	
	while(aux == 0){//se imprime toda las lista con el mismo nombre
		printf("so puta vida \n");
		struct dogType pet2;
		fread(&pet2,sizeof(struct dogType),1,fp);
		if(strcmp(to_lower(name),to_lower(pet2.name)) == 0){
			fprintf(fp2,"Nombre: %s\n",pet2.name);
			fprintf(fp2,"Tipo: %s\n",pet2.type);
			fprintf(fp2,"Edad: %i\n",pet2.age);
			fprintf(fp2,"Raza: %s\n",pet2.race);
			fprintf(fp2,"Tamaño: %i\n",pet2.height);
			fprintf(fp2,"Peso: %f\n",pet2.weight);
			fprintf(fp2,"Genero: %s\n\n",pet2.gender);	
		}else{
			aux = 1;
		}
	}
	fclose(fp2);	
	fclose(fp);
	system("gedit probe2.txt");
	remove("probe2.txt");
	return i;
}

void log_doc(int regis, int operat){
	char ip[12] = "192.168.0.6";
	FILE *fp;
	fp = fopen("serverDogs.log","a");
	time_t tiempo = time(0);
	struct tm *tlocal = localtime(&tiempo);
	char output[128];
	strftime(output,128,"%Y/%m/%d %H:%M:%S",tlocal);
	fprintf(fp,"\n %s ",output);
	fprintf(fp,"%s",ip);
	switch(operat){
		case 1:
			fprintf(fp," inserción");	
			break;
		case 2:
			fprintf(fp," lectura");	
			break;
		case 3:
			fprintf(fp," borrado");
			break;
		case 4:
			fprintf(fp," búsqueda");	
			break;
		default:
			break;		
	}
	fprintf(fp," %d",regis);
	fclose(fp);
}

void log_doc_name(char name[], int operat){
	char ip[12] = "192.168.0.6";
	FILE *fp;
	fp = fopen("serverDogs.log","a");
	time_t tiempo = time(0);
	struct tm *tlocal = localtime(&tiempo);
	char output[128];
	strftime(output,128,"%Y/%m/%d %H:%M:%S",tlocal);
	fprintf(fp,"\n %s ",output);
	fprintf(fp," %s",ip);
	switch(operat){
		case 1:
			fprintf(fp," insercion");	
			break;
		case 2:
			fprintf(fp," lectura");	
			break;
		case 3:
			fprintf(fp," borrado");
			break;
		case 4:
			fprintf(fp," busqueda");	
			break;
		default:
			break;		
	}
	fprintf(fp," %s",name);
	fclose(fp);
}


void *function(void *args) {// la función para todooooo
	char ass;

	int clientfd = *(int*)args;
	printf("%d \n ",clientfd);
	int r , position = 0, regiNum = 0, searchDat = 0,eleccion = 0, delDat = 0, plaSea1 = 0,plaSea2 = 0;
	int option = 1;
	char name[32];
	int hashList[1000000];
	for(int j = 0;j<1000000;j++)
		hashList[j]=0;
	hashing(hashList);
	typedef enum {true,false} bool;
	bool bucle = true;
	while (bucle == true){
		printf("estoy en menú\n");
		int optionInMenuAux = 0;
		int option = 0;
		int r;
		bool optionInMenu = true;
		r = recv(clientfd,&option,sizeof(int),0);
		switch(option){
			case 1:
				while (optionInMenu == true){
					printf("estoy en 1\n");
					struct dogType newPet;
					r = recv(clientfd,&newPet,sizeof(struct dogType),0);

					pthread_mutex_lock(&mux);//zona crítica
					sem_wait(semaforo);//zona crítica
					r = read(pipefd[0],&ass,CHAR);//zona crítica
					position = crear_registro(newPet);
					write(pipefd[1],"H",CHAR);//Fin
					sem_post(semaforo);//Fin
					pthread_mutex_unlock(&mux);//Fin

										
					printf("%d \n",position);
					r = send(clientfd ,&position ,sizeof(int),0);
					for(int j = 0;j<1000000;j++)//se vuelve a ejecutar la funcion de hash para agregar el nuevo elemento
						hashList[j]=0;
					
					pthread_mutex_lock(&mux);//zona crítica
					sem_wait(semaforo);//zona crítica
					r =read(pipefd[0],&ass,CHAR);//zona crítica

					hashing(hashList);

					write(pipefd[1],"H",CHAR);//Fin
					sem_post(semaforo);//Fin
					pthread_mutex_unlock(&mux);//Fin

					r = recv(clientfd,&optionInMenuAux,sizeof(int),0);
					if(optionInMenuAux==1){
						printf("salí de 1\n");
						optionInMenu = false;
					}

					pthread_mutex_lock(&mux);//zona crítica
					sem_wait(semaforo);//zona crítica
					r = read(pipefd[0],&ass,CHAR);//zona crítica
					log_doc(position,option);
					write(pipefd[1],"H",CHAR);//Fin
					sem_post(semaforo);//Fin
					pthread_mutex_unlock(&mux);//Fin
				}
				break;
			case 2:
				while (optionInMenu == true){
					printf("estoy en 2\n");
					regiNum = registers_number();
					r = send(clientfd ,&regiNum ,sizeof(int),0);
					r = recv(clientfd ,&searchDat ,sizeof(int) ,0 );

					pthread_mutex_lock(&mux);//zona crítica
					sem_wait(semaforo);//zona crítica
					r =read(pipefd[0],&ass,CHAR);//zona crítica
					buscar_registro(searchDat);//se llama a la función de buscar registro
					write(pipefd[1],"H",CHAR);//Fin
					sem_post(semaforo);//Fin
					pthread_mutex_unlock(&mux);//Fin

					r = recv(clientfd ,&eleccion ,sizeof(int) ,0 );
					if(eleccion == 1){
						changes(searchDat);
					}
					r = recv(clientfd,&optionInMenuAux,sizeof(int),0);
					if(optionInMenuAux==1){
						printf("salí de 2\n");
						optionInMenu = false;
					}
					
					pthread_mutex_lock(&mux);//zona crítica
					sem_wait(semaforo);//zona crítica
					r = read(pipefd[0],&ass,CHAR);//zona crítica
					log_doc(searchDat,option);
					write(pipefd[1],"H",CHAR);//Fin
					sem_post(semaforo);//Fin
					pthread_mutex_unlock(&mux);//Fin
				}
				break;
			case 3:
				while (optionInMenu == true){
					printf("estoy en 3\n");
					regiNum = registers_number();
					r = send(clientfd ,&regiNum ,sizeof(int),0);
					r = recv(clientfd ,&delDat ,sizeof(int) ,0 );

					pthread_mutex_lock(&mux);//zona crítica
					sem_wait(semaforo);//zona crítica
					r =read(pipefd[0],&ass,CHAR);//zona crítica
					eliminar_registro(delDat);//se llama a la función de eliminar
					write(pipefd[1],"H",CHAR);//Fin
					sem_post(semaforo);//Fin
					pthread_mutex_unlock(&mux);//Fin

					for(int j = 0;j<1000000;j++)// al eliminar un dato se actualiza el hash
						hashList[j]= 0;

					pthread_mutex_lock(&mux);//zona crítica
					sem_wait(semaforo);//zona crítica
					r =read(pipefd[0],&ass,CHAR);//zona crítica
					hashing(hashList);
					write(pipefd[1],"H",CHAR);//Fin
					sem_post(semaforo);//Fin
					pthread_mutex_unlock(&mux);//Fin

					r = recv(clientfd,&optionInMenuAux,sizeof(int),0);
					if(optionInMenuAux==1){
						printf("salí de 3\n");
						optionInMenu = false;
					}

					pthread_mutex_lock(&mux);//zona crítica
					sem_wait(semaforo);//zona crítica
					r = read(pipefd[0],&ass,CHAR);//zona crítica
					log_doc(delDat,option);
					write(pipefd[1],"H",CHAR);//Fin
					sem_post(semaforo);//Fin
					pthread_mutex_unlock(&mux);//Fin
				}
			
				break;
			case 4:
				while (optionInMenu == true){
					for(int j = 0;j<1000000;j++)//se vuelve a ejecutar la funcion de hash para agregar el nuevo elemento
						hashList[j]=0;

					pthread_mutex_lock(&mux);//zona crítica
					sem_wait(semaforo);//zona crítica
					r =read(pipefd[0],&ass,CHAR);//zona crítica
					hashing(hashList);
					write(pipefd[1],"H",CHAR);//Fin
					sem_post(semaforo);//Fin
					pthread_mutex_unlock(&mux);//Fin

					printf("estoy en 4\n");
					r = recv(clientfd ,name ,NAME ,0 );
					printf("%s",name);

					pthread_mutex_lock(&mux);//zona crítica
					sem_wait(semaforo);//zona crítica
					r =read(pipefd[0],&ass,CHAR);//zona crítica
					plaSea1 = search_i(hashList, name);//se llama a la función de buscar por nombre y retorna el primer encuentro
					write(pipefd[1],"H",CHAR);//Fin
					sem_post(semaforo);//Fin
					pthread_mutex_unlock(&mux);//Fin

					r = send(clientfd ,&plaSea1 ,sizeof(int),0);
					r = recv(clientfd,&optionInMenuAux,sizeof(int),0);
					if(optionInMenuAux==1){
						printf("salí de 4\n");
						optionInMenu = false;
					}

					pthread_mutex_lock(&mux);//zona crítica
					sem_wait(semaforo);//zona crítica
					r = read(pipefd[0],&ass,CHAR);//zona crítica
					log_doc_name(name,option);
					write(pipefd[1],"H",CHAR);//Fin
					sem_post(semaforo);//Fin
					pthread_mutex_unlock(&mux);//Fin
				}
				break;
			case 5:
				printf("morí\n");
				bucle = false;// simplemente lo rompe
				break;
			default:
				break;
		}
	}
	close(clientfd);
}


int main () {	
	pthread_t hilo[NUMTHREADS];
	int servfd,clientfd[NUMTHREADS];
	int r , position = 0, regiNum = 0, searchDat = 0,eleccion = 0, delDat = 0, plaSea1 = 0,plaSea2 = 0,i=0;
	int option = 1,tru=0;
	char name[32];

	pthread_mutex_init(&mux,NULL);//MUTEX

	semaforo = sem_open("semaforo1",O_CREAT,0700,SEMINIT);// se crea el SEMAFORO

	r = pipe(pipefd);//PILA
	write(pipefd[1],"H",CHAR);

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
	tamac=0;
	while(tru == 0){
		r = listen(servfd,BACKLOG);
		if(r == -1){
			perror("error en el listen");
			exit(-1);
		}
		
		clientfd[i] = accept(servfd,(struct sockaddr *)&cliente,&tamac); // se hace la conexión con el cliente y existe un arreglo de direcciones de cliente

		if(clientfd[i] == -1){
			perror("error en el accept");
			exit(-1);
		}
		r = pthread_create(&hilo[i],NULL, function, (void*)&clientfd[i]); // se crea el hilo y se envía la dirección del cliente 
		i++;
	}	
}

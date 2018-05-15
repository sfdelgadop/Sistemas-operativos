#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h> 
#include <semaphore.h> 
#include <fcntl.h>
#include <sys/stat.h>

#define MAX_PROCESOS 1
#define NUM_HILOS 8
#define SEMINIT 1

sem_t *semaforo;

struct datos_tipo{
	int dato;
	int p;
};

void proceso(void *datos){
	struct datos_tipo *datos_proceso;
	datos_proceso = (struct datos_tipo*) datos;
	int a, i, j, p;
	a = datos_proceso -> dato;
	p = datos_proceso -> p;
	sem_wait(semaforo);//
	for(i=0; i <= p; i++){
		printf(" %i ",a);
	}
	fflush(stdout);
	sleep(1);
	for(i=0; i<=p; i++)
	{
		printf("- ");
	}
	fflush(stdout);
	sem_post(semaforo);//
}

int main(){
	semaforo = sem_open("semaforo1",O_CREAT,0700,SEMINIT);// se crea el semaforo con el nombre tal 
	int error, i;
	char *valor_devuelto;

	/* Variables para hilos*/
	struct datos_tipo hilo_datos[NUM_HILOS];
	pthread_t idhilo[NUM_HILOS];
	
	for(i=0; i<NUM_HILOS; i++){
		hilo_datos[i].dato = i;
		hilo_datos[i].p = i+1;
	}

	/*se lanzan los hilos*/
	for(i=0; i<NUM_HILOS; i++){
		error=pthread_create(&idhilo[i], NULL, (void*)proceso, (void*)(&hilo_datos[i]));
		if (error != 0){
			perror ("No puedo crear hilo");
			exit (-1);
		}
	}
	/*Esperar a que terminen*/
	for(i=0; i<NUM_HILOS; i++){
		pthread_join(idhilo[i], (void **)&valor_devuelto);
	}
	sem_close(semaforo);//se cierra el semaforo
	sem_unlink("semaforo1");//mata al semaforo del kernel
	return 0;
}

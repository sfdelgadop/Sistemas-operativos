//atomización es una forma en la que el sistema operativo marca una sección código no puede ser interrumpido en su ejecución, se y lo hace continuamente
//lpthread
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h> 
#include <semaphore.h> 
#include <fcntl.h>
#include <sys/stat.h>

#define NUMTHREADS 8
#define SEMINIT 1

sem_t *semaforo;

void *function(void *args){
	sem_wait(semaforo);//
	printf("inicio hilo %i\n", *(int*)args);
	sleep(1);
	printf("fin hilo %i\n", *(int*)args);
	sem_post(semaforo);//
	
}

int main(){
	semaforo = sem_open("semaforo1",O_CREAT,0700,SEMINIT);// se crea el semaforo con el nombre tal 
	int r;

	pthread_t hilo[NUMTHREADS];
	int i, data[NUMTHREADS];

	for(i = 0; i <NUMTHREADS; i++){
		data[i] = i;
		r = pthread_create(&hilo[i],NULL, function, (void*)&data[i]);
	}

	if(r <0){
		perror("Error en el pthread");    
	}

	for(i = 0; i<NUMTHREADS; i++){
		r = pthread_join(hilo[i],NULL);    
	}
	sem_close(semaforo);//se cierra el semaforo
	sem_unlink("semaforo1");//mata al semaforo del kernel
}

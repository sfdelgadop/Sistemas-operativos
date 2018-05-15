//atomización es una forma en la que el sistema operativo marca una sección código no puede ser interrumpido en su ejecución, se y lo hace continuamente

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h> 

#define NUMTHREADS 8
#define CHAR 1

int pipefd [2];

void *function(void *args){
	char ass;
	int r;
	r =read(pipefd[0],&ass,CHAR);
	printf("inicio hilo %i\n", *(int*)args);
	sleep(1);
	printf("fin hilo %i\n", *(int*)args);
	write(pipefd[1],"H",CHAR);
	
}

int main(){
	int r;
	r = pipe(pipefd);

	pthread_t hilo[NUMTHREADS];
	int i, data[NUMTHREADS];

	write(pipefd[1],"H",CHAR);

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
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUMTHREADS 8

//lpthread
void *function(void *args){

	printf("\n%i", *(int*)args);

}

int main(){

    pthread_t hilo[NUMTHREADS];
    int r, i, data[NUMTHREADS];

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

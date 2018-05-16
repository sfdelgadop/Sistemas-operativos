CC = gcc

all: ejecutable_serv ejecutar

ejecutar: ejecutable_serv
	reset
	./ejecutable_serv

ejecutable_serv: p1-dogProgram_server.c
	$(CC) p1-dogProgram_server.c -o ejecutable_serv

ejecutar: ejecutable_client
	./ejecutable_client

ejecutable_serv: p1-dogProgram_client.c
	$(CC) p1-dogProgram_client.c -o ejecutable_client


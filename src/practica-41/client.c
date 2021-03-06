#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define DNI 0x72106540L
#define GET 30
#define PORT 3000
#define PORT2 3001
#define BUFFER 256

int main(int argc, char** argv){
	char gets[GET], buffer[BUFFER];

	printf("Ejercicio 1, inserta para continuar:\n");
	fgets(gets,GET,stdin);
	
	//CLIENTE
	int sock;
	struct sockaddr_in server;

	//SERVIDOR
	int socket_desc , client_sock , c , read_size;
    	struct sockaddr_in server2 , client;
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(PORT);
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror ("Error creando el socket...\n");
		exit (1);
	}
	
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
        	perror("Conexion fallida...\n");
        	exit(0);
	}
	
	printf("Conectado!!!\n");

	printf("Ejercicio 2, inserta para continuar:\n");
	fgets(gets,GET,stdin);
	
	if( recv(sock , buffer , BUFFER , 0) < 0)
        {
            perror("Error al recibir mensaje");
            exit(0);
        }
	
	printf("secreto 2 -> %s\n",buffer);

	printf("Ejercicio 3, inserta para continuar:\n");
	fgets(gets,GET,stdin);
	
	if( send(sock , buffer , strlen(buffer) , 0) < 0)
        {
            perror("Envio fallido...\n");
            exit(0);
        }

	if( recv(sock , buffer , BUFFER , 0) < 0)
        {
            perror("Error al recibir mensaje");
            exit(0);
        }
	
	printf("secreto 3 -> %s\n",buffer);
	
	if( send(sock , buffer , strlen(buffer) , 0) < 0)
        {
            perror("Envio fallido...\n");
            exit(0);
        }
	
	printf("Ejercicio 4, inserta para continuar(EJECUTAR AQUI ANTES):\n");
	fgets(gets,GET,stdin);
	
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    	if (socket_desc == -1){
        	printf("Could not create socket");
    	}

	server2.sin_family = AF_INET;
    	server2.sin_addr.s_addr = INADDR_ANY;
    	server2.sin_port = htons( PORT2 );
	
	if( bind(socket_desc,(struct sockaddr *)&server2 , sizeof(server)) < 0){
        	perror("Error en el bind");
        	exit(0);
    	}
	
	printf("Bind realizado\n");
	listen(socket_desc , 3);
	printf("Listen realizado\n");
	
	c = sizeof(struct sockaddr_in);
	if(accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)<0){
        	perror("accept fallido...\n");
        	exit(0);
    	}
	
	printf("Ejercicio 5, pulsa para continuar:\n");
	fgets(gets,GET,stdin);

	if( recv(sock , buffer , BUFFER , 0) < 0)
        {
            perror("Error al recibir mensaje");
            exit(0);
        }

	printf("Mensaje a enviar -> %s\n",buffer);

	if( send(sock , buffer , strlen(buffer) , 0) < 0)
        {
            perror("Envio fallido...\n");
            exit(0);
        }

	printf("Ejercicio 6, pulsa para continuar:\n");
	fgets(gets,GET,stdin);
	
	struct sockaddr_in si_me, si_other;
	int s, i, slen = sizeof(si_other) , recv_len;
	
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
		perror("Error al crear el socket...\n");
		exit(0);
    	}
	
	memset((char *) &si_me, 0, sizeof(si_me));
	
	si_me.sin_family = AF_INET;
    	si_me.sin_port = htons(PORT);
    	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1){
        	perror("Error al hace bind...\n");
		exit(0);
	}
	
	strcmp("HOLA!",buffer);

	if (sendto(s, buffer,strlen(buffer), 0, (struct sockaddr*) &si_other, slen) == -1)
        {
		perror("Error enviando HOLA...\n");
		exit(0);
        }

	printf("Ejercicio 7, inserta para continuar(EJECUTAR ANTES AQUI):\n");
	fgets(gets,GET,stdin);
	
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
		printf("Error al crear el socket...\n");
		exit(0);
        }

        memset((char *) &si_me, 0, sizeof(si_me));

        si_me.sin_family = AF_INET;
        si_me.sin_port = htons(PORT2);
        si_me.sin_addr.s_addr = htonl(INADDR_ANY);

        if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1){
                perror("Error al hace bind...\n");
                exit(0);
        }
	
	if ((recv_len = recvfrom(s, buffer, BUFFER, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
		perror("Error al recibir...\n");
		exit(0);
        }
	
	if (sendto(s, buffer,strlen(buffer), 0, (struct sockaddr*) &si_other, slen) == -1)
        {
                perror("Error enviando respuesta...\n");
                exit(0);
        }
	
	printf("EJERSISIO TERMINADO WEY!!!!!!!!!\n");
}

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
	server.sin_addr.s_addr = INADDR_ANY;
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
	
	if( recv(sock , buffer , BUFFER , 0) < 0)
        {
            perror("Error al recibir mensaje");
            exit(0);
        }
	int secreto = *((int*)buffer);
	sprintf(buffer,"<%d>",secreto);
	printf("secreto 2 -> %s\n",buffer);
	
	if( send(sock , buffer , strlen(buffer) , 0) < 0)
        {
            perror("Envio fallido...\n");
            exit(0);
        }

	close(sock);
	printf("Ejercicio 2, inserta para continuar:\n");
        fgets(gets,GET,stdin);
	
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    	if (socket_desc == -1){
        	printf("Could not create socket");
    	}
	struct sockaddr_in client_in;	
	memset(&server2,0,sizeof(server2));
	server2.sin_family = AF_INET;
    	server2.sin_addr.s_addr = INADDR_ANY;
    	server2.sin_port = htons( PORT2 );
	
	if( bind(socket_desc,(struct sockaddr *)&server2 , sizeof(struct sockaddr_in)) < 0){
        	perror("Error en el bind");
        	exit(0);
    	}
	
	printf("Bind realizado\n");
	if(listen(socket_desc , 5)==-1)
	{
		perror("Error en el listen");
		exit(0);
	}
	printf("Listen realizado\n");
	printf("Inicio bucle...\n");
	int fd;
	c = sizeof(struct sockaddr_in);
	int pidserver;
	if((pidserver=fork())==0) {
		while(1) {
			if((fd=accept(socket_desc, (struct sockaddr *)&client_in,&c))<0){
        			perror("accept fallido...\n");
        			exit(0);
    			}
			printf("Creando hijo...\n");
			if(fork()==0) {
				printf("Hijo con PID->%d empezando\n");
				close(socket_desc);
				if( recv(fd , buffer , BUFFER , 0) < 0)
				{
					perror("Error al recibir mensaje");
					exit(0);
				}
				int secreto = *((int*)buffer);
				sprintf(buffer,"<%d>",secreto);
				printf("secreto 2 -> %s\n",buffer);

				if( send(fd , buffer , strlen(buffer) , 0) < 0)
				{
					perror("Envio fallido...\n");
					exit(0);
				}
				close(fd);
				printf("Hijo con PID->%d ha terminado");
				exit(0);
			}
		}
	}
	printf("Pulsa para terminar:\n");
	fgets(gets,GET,stdin);
	
	kill(pidserver,SIGINT);
	int status;
	waitpid(pidserver,&status,0);

	printf("Ejercicio 3:\n");
	fgets(gets,GET,stdin);

	printf("EJERSISIO TERMINADO WEY!!!!!!!!!\n");
}

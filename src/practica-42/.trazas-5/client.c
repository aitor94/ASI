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
#include <signal.h>

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
	printf("Inicio bucle...\n");
	int fd;
	c = sizeof(struct sockaddr_in);
	int pidserver,pid = 5;
	if((pidserver=fork())==0){
	while(1==1){
		if(listen(socket_desc , 5)==-1){
                	perror("Error en el listen");
                	exit(0);
        	}
		if((fd=accept(socket_desc, (struct sockaddr *)&client_in,&c))<0){
        		perror("accept fallido...\n");
       			exit(0);
		}
		printf("Creando hijo...\n");
			if((pid=fork())==0){
				printf("Hijo con PID->%d empezando\n",getpid());
				if( recv(fd , buffer , BUFFER , 0) < 0){
					perror("Error al recibir mensaje");
					exit(0);
				}
				int secreto = *((int*)buffer);
				sprintf(buffer,"<%d>",secreto);
				printf("secreto 2 -> %s\n",buffer);

				if( send(fd , buffer , strlen(buffer) , 0) < 0){
					perror("Envio fallido...\n");
					exit(0);
				}
				recv(fd,buffer,BUFFER,0);//con esta mierda puedo esperar a que cierre el otro lado
				close(fd);
				exit(0);	
		}
		}
		close(fd);
	}
	printf("Pulsa para terminar:\n");
	fgets(gets,GET,stdin);
	kill(pidserver,SIGINT);
	int status;
	waitpid(pidserver,&status,0);
	
	printf("Ejercicio 3, pulsa para continuar:\n");
	fgets(gets,GET,stdin);
	
	struct sockaddr_in si_me, si_other;

        bzero ((char *) &si_me, sizeof(struct sockaddr_in));
        bzero ((char *) &si_other, sizeof(struct sockaddr_in));

        int s, i, slen = sizeof(si_other) , recv_len;
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
                perror("Error al crear el socket...\n");
                exit(0);
        }

        memset((char *) &si_me, 0, sizeof(si_me));

        si_me.sin_family = AF_INET;
        si_me.sin_port = htons(50000);
        si_me.sin_addr.s_addr = htonl(INADDR_ANY);

        si_other.sin_family = AF_INET;
        si_other.sin_port = htons(PORT);
        si_other.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1){
                perror("Error al hace bind...\n");
                exit(0);
        }

	char msg[5];
	msg[0]='<';
	msg[1]='?';
	msg[2]='?';
	msg[3]='?';
	msg[4]='>';
        if (sendto(s,msg,5, 0,(struct sockaddr *) &si_other, slen) == -1)
        {
                perror("Error enviando <?>...\n");
                exit(0);
        }
	printf("Enviado");
	if (recvfrom(s,buffer,BUFFER,0,(struct sockaddr *) &si_other, &slen)<0){
		perror("Error recibiendo...\n");
		exit(0);
	}
	printf("%s",buffer);
	int sec8;
	sscanf(buffer,"<%d>",&sec8);

	if (sendto(s,buffer,strlen(buffer), 0,(struct sockaddr *) &si_other, slen) == -1)
        {
                perror("Error enviando secreto...\n");
                exit(0);
        }
	
	printf("Ejercicio 4, pulsa para continuar:\n");
	fgets(gets,GET,stdin);

//**************************************************************************************
//***************************************************************************************
}

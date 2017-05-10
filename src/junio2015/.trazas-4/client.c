#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/wait.h>
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
#define PORT2 3010
#define BUFFER 256

void sigchld_handler(int s)
{
    while(wait(NULL) > 0);
}

int main(int argc, char** argv){
	char ge[GET], buffer[BUFFER];
	struct sockaddr_in server;
	int socket_desc, fd, c;

	printf("Comenzar:");
	fgets(ge,GET,stdin);

	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc<0){
		printf("Error creando socket\n");
		exit(0);
	}

	memset(&server,0,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT2);

	if(bind(socket_desc,(struct sockaddr *)&server,sizeof(struct sockaddr_in))<0){
		printf("Error en el bind");
		exit(0);
	}
	
	printf("Inicio bucle.....\n");
	c = sizeof(struct sockaddr_in);
	struct sockaddr_in client_in;
	int pid;
	signal(SIGCHLD , sigchld_handler);

	while(1){
		if(listen(socket_desc,5)<0){
			printf("Error al hacer listen\n");
			exit(0);
		}
		if((fd=accept(socket_desc, (struct sockaddr *)&client_in,&c))<0){
			printf("Error en accept\n");
			exit(0);
		}
		printf("Conexion recibida........\n");
		char user[BUFFER];	
		if((pid=fork())==0){
			close(socket_desc);
			int intentos = 0;//gcc -L . aasdf.c asdfasdf.o -l mylib->libmylib
			int pd = getpid();//#include ""se le puede meter un path
			char pid[10];
			sprintf(pid,"%d\n",pd);
			while(intentos!=3){
				if(send(fd,"login:",strlen("login:"),0)<0){
					printf("Envio login fallido");
					exit(0);
				}
			
				if(recv(fd,user,BUFFER,0)<0){
					printf("Error recibiendo usuario");
					exit(0);
				}
			
				printf("Usuario recibido->%s",user);
			
				if(send(fd,"passwd:",strlen("passwd:"),0)<0){
                                	printf("Envio login fallido\n");
                                	exit(0);
                        	}
				printf("Passwd enviado\n");
						
				if(recv(fd,buffer,BUFFER,0)<0){
                                	printf("Error recibiendo usuario\n");
                                	exit(0);
                        	}
				printf("Contrasenia recibida->%s\n",buffer);
				
				if(strcmp("anonymous",strtok(user,"\n"))==0 && strcmp("guest",strtok(buffer,"\n"))==0 ){
					printf("Usuario correcto\n");
					sprintf(buffer,"%s:$",user);
					if(send(fd,buffer,strlen(buffer),0)<0){
                                        	printf("Envio user fallido\n");
                                        	exit(0);
                                	}
					//ej2****************************
					recv(fd,buffer,BUFFER,0);
					if(strcmp("whoami",strtok(buffer,"\n"))==0)
						send(fd,"anonymous\n",strlen("anonymous\n"),0);
					else if(strcmp("pid",strtok(buffer,"\n"))==0)
						send(fd,pid,strlen(pid),0);
						
				}
				else{
					intentos++;
					printf("Usuario erroneo->%d\n",intentos);
				}
			}//senial sigchild strtok si en un read de tcp recibo 0 la conexion se ha cerrado
			close(fd);
			exit(0);		
		}
		close(fd);
	}
}

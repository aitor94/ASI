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
	int memoria = shmget(DNI,1024,IPC_CREAT|0666);
	int *segmento;
	segmento = shmat(memoria,NULL,0);

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
		segmento[0]++;
		//printf("Conexion recibida........\n");
		char user[BUFFER];	
		if((pid=fork())==0){
			close(socket_desc);
			int intentos = 0;//gcc -L . aasdf.c asdfasdf.o -l mylib->libmylib
			int pd = getpid();//#include ""se le puede meter un path
			char pid[10];
			
			while(intentos!=4){
				if(send(fd,"login:",strlen("login:"),0)<0){
					printf("Envio login fallido");
					exit(0);
				}
			
				if(recv(fd,user,BUFFER,0)<0){
					printf("Error recibiendo usuario");
					exit(0);
				}
			
				if(send(fd,"passwd:",strlen("passwd:"),0)<0){
                                	printf("Envio login fallido\n");
                                	exit(0);
                        	}
						
				if(recv(fd,buffer,BUFFER,0)<0){
                                	printf("Error recibiendo usuario\n");
                                	exit(0);
                        	}
				
				if(strcmp("anonymous",strtok(user,"\n"))==0 && strcmp("guest",strtok(buffer,"\n"))==0 ){
					segmento[1]++;
					printf("Usuario correcto\n");
					int rec;
					int active = 0;
					while(1){
						sprintf(buffer,"%s:$",user);
						if(send(fd,buffer,strlen(buffer),0)<0){
                                        		printf("Envio user fallido\n");
                                        		exit(0);
                                		}
						if(active==0){
							active = 1;
							segmento[3]++;
						}
						rec = recv(fd,buffer,BUFFER,0);
						if(rec==0){
							segmento[3]--;
							close(fd);
							exit(0);
						}
						sprintf(pid,"%d\n",pd);
						if(strcmp("whoami",strtok(buffer,"\n"))==0){
							send(fd,"anonymous\n",strlen("anonymous\n"),0);
						}
						else if(strcmp("pid",strtok(buffer,"\n"))==0){
							send(fd,pid,strlen(pid),0);
						}
						else if(strcmp("quit",strtok(buffer,"\n"))==0){
							segmento[3]--;
							close(fd);
							exit(0);
						}
						else if(strcmp("ncnx",strtok(buffer,"\n"))==0){
							sprintf(buffer,"%d\n",segmento[0]);
							send(fd,buffer,strlen(buffer),0);
						}
						else if(strcmp("nsess",strtok(buffer,"\n"))==0){
							sprintf(buffer,"%d\n",segmento[1]);
							send(fd,buffer,strlen(buffer),0);
						}
						else if(strcmp("nok",strtok(buffer,"\n"))==0){
							sprintf(buffer,"%d\n",segmento[2]);
							send(fd,buffer,strlen(buffer),0);
						}
						else if(strcmp("active session",strtok(buffer,"\n"))==0){
							sprintf(buffer,"%d\n",segmento[3]);
							send(fd,buffer,strlen(buffer),0);
						}		
						else {
							send(fd,"Error\n",strlen("Error\n"),0);
						}
					}
				}		
				else{
					intentos++;
					segmento[2]++;
					printf("Intento fallido pid=%d -> %d\n",getpid(),intentos);
				}
			}//senial sigchild strtok si en un read de tcp recibo 0 la conexion se ha cerrado
			//printf("Cerrando por fallos %d",getpid());
			close(fd);
			exit(0);		
		}
		close(fd);
	}
}

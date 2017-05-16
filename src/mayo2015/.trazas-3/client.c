#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define DNI 0x72106540L
#define BUFFER 256
#define GET 30
#define PORT 3010

void mata();

int main(int argc, char**argv[]){
	char ge[GET];

	int memoria = shmget(DNI,1024,IPC_CREAT|0666);
	char * segmento = shmat(memoria,NULL,0);

	struct sockaddr_in server, client;
	memset(&server,0,sizeof(server));

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(PORT);

	int s = socket(AF_INET,SOCK_STREAM,0);
	
	bind(s,(struct sockaddr *)&server,sizeof(server));
	
	int l = sizeof(client);
	listen(s,5);

	signal(SIGCHLD,mata);
	int fd;
	printf("Esperando en accept\n");
	while((fd=accept(s,(struct sockaddr *)&client,&l))!=-1){
		if(fork()==0){
			segmento = shmat(memoria,0,IPC_CREAT|0666);
			close(s);
			char dato[BUFFER],tipo,buffer[BUFFER];
			int dsp;
			int r;
			while((r=read(fd,buffer,BUFFER))>0){
				printf("Recibido %s\n",buffer);
				if(strcmp("hello!!\n",buffer)==0)
					write(fd,"OK\n",strlen("OK\n"));	
				sscanf(buffer,"<%d>%c",&dsp,&tipo);
				printf("dsp=%d, tipo=>%c",dsp,tipo);
				if(tipo=='?'){
					sprintf(buffer,"%d\n",*(int *)(segmento+dsp));
					write(fd,buffer,strlen(buffer));
				}
				if(tipo=='='){
					int num;
					sscanf(buffer,"<%d>%c%d",&dsp,&tipo,&num);
					printf("Numero recogido->%d",num);
					sprintf((segmento+dsp),"%d",num);
					//*((int *)(segmento+dsp))=num;
					send(fd,"\n",1,0);
				}
				if(tipo=='+'){
					int num1,num2,desp1,desp2;
					sscanf(buffer,"<%d>%c<%d>",&desp1,&tipo,&desp2);
					num1 = *((int *)(segmento+desp1));
					num2 = *((int *)(segmento+desp2));

					sprintf(buffer,"%d\n",(num1+num2));
					send(fd,buffer,strlen(buffer),0);
				}
			}
			printf("Hijo terminando-> %d\n",r);
			close(fd);
			exit(0);
		}
		close(fd);
	}
}

void mata(){
	int status;
	wait(&status);
}

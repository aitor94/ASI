#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>

#define DNI 0x72106540L
#define BUFFER 256
#define GET 30

int cola, pidmonitor;

typedef struct msg_cola
{
	long id;
	char tipo;
	char cadena[BUFFER];
}msg_cola;

void saludo();
void respuesta();

int main(int argc, char ** argv){
	char ge[GET], buffer[BUFFER];
	int hijo1, hijo2, memoria;
	msg_cola msg;	
	pidmonitor = atoi(argv[1]);	
	
	memoria = shmget(DNI,1024,IPC_CREAT|0666);

	char * segmento;
	segmento = shmat(memoria,NULL,0);

	signal(SIGUSR1,saludo);	
	signal(	SIGUSR2,respuesta);	

	printf("Iniciar examen:");
	fgets(ge,GET,stdin);

	cola = msgget(DNI,IPC_CREAT|0666);
	sleep(1);

	kill(pidmonitor,SIGUSR1);
	
	if((hijo1=fork())==0){
		signal(SIGUSR1,saludo);
		signal(SIGUSR2,respuesta);
		printf("Hijo 1 habilitado\n");
		segmento = shmat(memoria,NULL,0);
		pause();
                while(msgrcv(cola,(struct msgbuf *)&msg,sizeof(msg.cadena)+1,getpid(),0)){
                        printf("Mensaje recibido en hijo1\n");
                        if(msg.tipo=='E'){
                                msg.id = 1L;
                                msg.tipo = 'e';
                                int dato = *((int*) msg.cadena);
                                sprintf(buffer,"<%d><%d>",getpid(),dato);
				strcpy(msg.cadena,buffer);
                                msgsnd(cola,(struct msgbuf *)&msg,strlen(msg.cadena)+1,0);
                        }
			else if(msg.tipo=='R'){
				int desp =*((int *) msg.cadena);
				int dato = *((int *)segmento+desp);
				*((long *)segmento + desp) = (long) getpid();
				sprintf(buffer,"<%d><%d>",getpid(),dato);
				strcpy(msg.cadena,buffer);
				msgsnd(cola,(struct msgbuf *)&msg,strlen(msg.cadena)+1,0);
                
			}
		}
	}
	else if((hijo2=fork())==0){
		signal(SIGUSR1,saludo);
		signal(SIGUSR2,respuesta);
		printf("Hijo 2 habilitado\n");
		segmento = shmat(memoria,NULL,0);
		pause();
                while(msgrcv(cola,(struct msgbuf *)&msg,sizeof(msg.cadena)+1,getpid(),0)){
                        printf("Mensaje recibido en hijo2\n");
                        if(msg.tipo=='E'){
                                msg.id = 1L;
                                msg.tipo = 'e';
                                int dato = *((int*) msg.cadena);
                                sprintf(buffer,"<%d><%d>",getpid(),dato);
				strcpy(msg.cadena,buffer);
                                msgsnd(cola,(struct msgbuf *)&msg,strlen(msg.cadena)+1,0);
                        }
			else if(msg.tipo=='R'){
                                int desp =*((int *) msg.cadena);
                                int dato = *((int *)segmento+desp);
                                *((long *)segmento + desp) = (long) getpid();
                                sprintf(buffer,"<%d><%d>",getpid(),dato);
                                strcpy(msg.cadena,buffer);
                                msgsnd(cola,(struct msgbuf *)&msg,strlen(msg.cadena)+1,0);
                	}
                }
	}
	else{
		pause();
		while(msgrcv(cola,(struct msgbuf *)&msg,sizeof(msg.cadena)+1,getpid(),0)){
			printf("Mensaje recibido en padre\n");
			if(msg.tipo=='E'){
                		msg.id = 1L;
                		msg.tipo = 'e';
                		int dato = *((int*) msg.cadena);
                		sprintf(buffer,"<%d><%d>",getpid(),dato);
				strcpy(msg.cadena,buffer);
                		msgsnd(cola,(struct msgbuf *)&msg,strlen(msg.cadena)+1,0);
        		}
			else if(msg.tipo=='R'){
                                int desp =*((int *) msg.cadena);
                                int dato = *((int *)segmento+desp);
                                *((long *)segmento + desp) = (long) getpid();
                                sprintf(buffer,"<%d><%d>",getpid(),dato);
                                strcpy(msg.cadena,buffer);
                                msgsnd(cola,(struct msgbuf *)&msg,strlen(msg.cadena)+1,0);
 
                	 }
		}
		printf("Pulsa para terminar:");
		fgets(ge,GET,stdin);

		msgctl(cola,IPC_RMID,(struct msqid_ds*)	NULL);
		int status;
		kill(hijo1,SIGINT);
		kill(hijo2,SIGINT);
		wait(&status);
		wait(&status);
		exit(0);
	}	
}

void respuesta(){
	printf("SIGUSR2 recibido %d\n",getpid());
	msg_cola msg;
	char buffer[BUFFER];

	msgrcv(cola,(struct msgbuf *)&msg,sizeof(msg.cadena)+1,getpid(),0);
	if(msg.tipo=='E'){
		msg.id = 1L;
		msg.tipo = 'e';
		int dato = *((int*) msg.cadena);
		sprintf(buffer,"<%d><%d>",getpid(),dato);
		msgsnd(cola,(struct msgbuf *)&msg,strlen(msg.cadena)+1,0);
	}	
}

void saludo(){
	printf("SIGUSR1 recibido %d\n",getpid());
	msg_cola msg;
	msg.id = 1L;
	msg.tipo = 'H';
	
	sprintf(msg.cadena,"<%d>",getpid());
	msgsnd(cola,(struct msgbuf *)&msg,strlen(msg.cadena)+1,0);	
}

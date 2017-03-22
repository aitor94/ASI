#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define TAM 30
#define BUFF 100
#define FIFO1 "/tmp/fifo_monitor_1"
#define FIFO2 "/tmp/fifo_monitor_2"
#define COLA 0x72106540L


typedef struct msg_cola
{
	long id;
	char datos[BUFF];
}msg_cola;


int main(int argc,char **argv)
{
	int fifo1,fifo2,pid,bytes;
	char cadena[TAM];
	char buffer[BUFF];
	msg_cola msg;

	if(argc!=2)
	{
		printf("Argumentos de entrada no validos");
		exit(0);
	}
	else
		pid=atoi(argv[1]);

	printf("pid monitor->%d\n",pid);	

	printf("Ejercicio 1:\n");
	fgets(cadena,TAM,stdin);

	fifo1=open(FIFO1,O_RDWR);

	if(fifo1==-1)
	{
		printf("Error al abrir la fifo");
		exit(0);
	}
	
	bytes=read(fifo1,buffer,BUFF);

	char secreto1[bytes];
	strcpy(secreto1,buffer);

	printf("Secreto1 = %s\n",secreto1);
	
	printf("Ejercicio 2:\n");
	fgets(cadena,TAM,stdin);

	if((mkfifo(FIFO2,S_IRWXU))==-1)
	{
		printf("Error creando fifo");
		exit(0);
	}

	fifo2=open(FIFO2,O_RDWR);
	
	if(fifo2==-1)
	{
		printf("Error abriendo fifo2");
		exit(0);
	}

	bytes=write(fifo2,secreto1,bytes);

	bytes=read(fifo1,buffer,BUFF);

	char secreto2[bytes];
	strcpy(secreto2,buffer);

	printf("Secreto 2 = %s\n",secreto2);
	
	bytes=write(fifo2,secreto2,bytes);

	printf("Ejercicio 3:\n");
	fgets(cadena,TAM,stdin);

	int cola=msgget(COLA,0666|IPC_CREAT);
	if(cola==-1)
	{
		printf("Error al abrir cola");
		exit(0);
	}

	int errc=msgrcv(cola,(struct msgbuf *)&msg,sizeof(msg.id)+sizeof(msg.datos),0,0);
	//el primer 0 es para que reciba cualquier mensaje sin mirar el tipo	

	
	if(errc==-1)
	{
		printf("Error leyendo al cola");
		exit(0);
	}	
	
	char secreto3[TAM];
	sprintf(secreto3,"<%ld>",msg.id);

	char secreto4[strlen(msg.datos)];
	strcpy(secreto4,msg.datos);

	printf("Secreto 3 = %s\n",secreto3);
	printf("Secreto 4 = %s\n",secreto4);

	printf("Ejercicio 4:\n");
	fgets(cadena,TAM,stdin);

	cola=msgget(COLA,0666|IPC_CREAT);
	if(cola==-1)
	{
		printf("Error al crear la cola");
		exit(0);
	}
	
	sprintf(buffer,"%s%s",secreto3,secreto4);

	printf("Mensaje a enviar = %s",buffer);
	
	msg.id=1;
	strcpy(msg.datos,buffer);

	errc=msgsnd(cola,(struct msgbuf *)&msg,strlen(msg.datos),0);
	if(errc==-1)
	{
		printf("Error enviando mensaje");
		exit(0);
	}	

	printf("Ejercicio 5:\n");
	fgets(cadena,TAM,stdin);

	cola=msgget(COLA,0666|IPC_CREAT);

	long canal=(long) getpid();

	errc=msgrcv(cola,(struct msgbuf *)&msg,sizeof(msg.id)+sizeof(msg.datos),canal,0);
	if(errc==-1)
	{
		printf("Error recibiendo de la cola");
		exit(0);
	}
	
	char secreto6[TAM];
	strcpy(secreto6,msg.datos);

	printf("Secreto 6 = %s\n",secreto6);
	
	printf("Ejercicio 6:\n");
	fgets(cadena,TAM,stdin);
	
	cola=msgget(COLA,0666|IPC_CREAT);

	canal=(long) pid;
	strcpy(msg.datos,secreto6);
	msg.id=canal;
	errc=msgsnd(cola,(struct msgbuf *)&msg,strlen(secreto6),0);
	if(errc==-1)
	{
		printf("Error al enviar");
		exit(0);
	}
	printf("Ejercicio 7:");
	fgets(cadena,TAM,stdin);
	msg.id=canal;

	errc=msgsnd(cola,(struct msgbuf *)&msg,strlen(secreto6),0);
	if(errc==-1)
	{
		printf("Error al enviar");
		exit(0);
	}

	unlink(FIFO1);
	unlink(FIFO2);
	errc=msgctl(cola,IPC_RMID,(struct msqid_ds*)NULL);
	if(errc==-1)
		printf("Error al destruir la cola");
}
